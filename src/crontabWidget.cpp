/**************************************************************************
 *   KT main GUI view implementation                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "crontabWidget.h"

#include <stdlib.h>
#include <unistd.h>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QApplication>
#include <QClipboard>

#include <KLocalizedString>
#include <QIcon>
#include <QAction>
#include <kstandardaction.h>

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"
#include "ctGlobalCron.h"

#include "crontabPrinter.h"
#include "tasksWidget.h"
#include "taskWidget.h"

#include "variableWidget.h"
#include "variableWidget.h"

#include "kcmCron.h"

#include "logging.h"

class CTGlobalCron;

class CrontabWidgetPrivate {
public:

	/**
	 * The application.
	 */
	CTHost* ctHost;

	/**
	 * Tree view of the crontab tasks.
	 */
	TasksWidget* tasksWidget;

	/**
	 * Tree view of the crontab tasks.
	 */
	VariablesWidget* variablesWidget;

	QAction* cutAction;
	QAction* copyAction;
	QAction* pasteAction;

	/**
	 * Clipboard tasks.
	 */
	QList<CTTask*> clipboardTasks;

	/**
	 * Clipboard variable.
	 */
	QList<CTVariable*> clipboardVariables;

	QRadioButton* currentUserCronRadio;
	QRadioButton* systemCronRadio;
	QRadioButton* otherUserCronRadio;

	QComboBox* otherUsers;

	/**
	 * Pointer to the pseudo Global Cron object
	 */
	CTGlobalCron* ctGlobalCron;


};

CrontabWidget::CrontabWidget(QWidget* parent, CTHost* ctHost) :
	QWidget(parent), d(new CrontabWidgetPrivate()) {

	d->tasksWidget = NULL;
	d->variablesWidget = NULL;

	d->ctHost = ctHost;

	if (d->ctHost->isRootUser()) {
		d->ctGlobalCron = new CTGlobalCron(d->ctHost);
	}
	else {
		d->ctGlobalCron = NULL;
	}

	setupActions();

	initialize();

	logDebug() << "Clipboard Status " << hasClipboardContent() << endl;

	d->tasksWidget->setFocus();

	QTreeWidgetItem* item = d->tasksWidget->treeWidget()->topLevelItem(0);
	if (item != NULL) {
		logDebug() << "First item found" << d->tasksWidget->treeWidget()->topLevelItemCount() << endl;
		item->setSelected(true);
	}

	d->tasksWidget->changeCurrentSelection();
	d->variablesWidget->changeCurrentSelection();

}

CrontabWidget::~CrontabWidget() {
	delete d->tasksWidget;
	delete d->variablesWidget;

	delete d->ctGlobalCron;

	delete d;
}

bool CrontabWidget::hasClipboardContent() {
	if (d->clipboardTasks.isEmpty() == false)
		return true;

	if (d->clipboardVariables.isEmpty() == false)
		return true;

	return false;
}

QHBoxLayout* CrontabWidget::createCronSelector() {
	QHBoxLayout* layout = new QHBoxLayout();

	layout->addWidget(new QLabel(i18n("Show the following Cron:"), this));

	QButtonGroup* group = new QButtonGroup(this);

	d->currentUserCronRadio = new QRadioButton(i18n("Personal Cron"), this);
	d->currentUserCronRadio->setChecked(true);
	group->addButton(d->currentUserCronRadio);
	layout->addWidget(d->currentUserCronRadio);

	d->systemCronRadio = new QRadioButton(i18n("System Cron"), this);
	group->addButton(d->systemCronRadio);
	layout->addWidget(d->systemCronRadio);

	d->otherUserCronRadio = new QRadioButton(i18n("Cron of User:"), this);
	group->addButton(d->otherUserCronRadio);

	d->otherUsers = new QComboBox(this);

	layout->addWidget(d->otherUserCronRadio);
	layout->addWidget(d->otherUsers);

	if (ctHost()->isRootUser()) {
		QStringList users;

		foreach(CTCron* ctCron, ctHost()->crons) {
			if (ctCron->isCurrentUserCron())
				continue;

			if (ctCron->isSystemCron())
				continue;

			users.append(ctCron->userLogin());
		}

		users.sort();
		d->otherUsers->addItems(users);
		d->otherUsers->addItem(QIcon::fromTheme( QLatin1String( "users") ), i18n("Show All Personal Crons"));
	} else {
		d->otherUserCronRadio->hide();
		d->otherUsers->hide();
	}

	connect(group, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &CrontabWidget::refreshCron);
	connect(d->otherUsers, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CrontabWidget::checkOtherUsers);

	layout->addStretch(1);

	return layout;
}

void CrontabWidget::initialize() {
	QVBoxLayout* layout = new QVBoxLayout(this);

	logDebug() << "Begin view refresh" << endl;

	logDebug() << "Creating Tasks list..." << endl;

	QHBoxLayout* cronSelector = createCronSelector();
	layout->addLayout(cronSelector);

	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	layout->addWidget(splitter);

	d->tasksWidget = new TasksWidget(this);
	splitter->addWidget(d->tasksWidget);
	splitter->setStretchFactor(0, 2);

	d->variablesWidget = new VariablesWidget(this);
	splitter->addWidget(d->variablesWidget);
	splitter->setStretchFactor(1, 1);

	refreshCron();

}

void CrontabWidget::refreshCron() {

	CTCron* ctCron = currentCron();

	d->tasksWidget->refreshTasks(ctCron);
	d->variablesWidget->refreshVariables(ctCron);

	if (ctCron->isMultiUserCron() && ctHost()->isRootUser()==false) {
		logDebug() << "Disabling view..." << endl;

		d->tasksWidget->treeWidget()->setEnabled(false);
		d->variablesWidget->treeWidget()->setEnabled(false);

		toggleNewEntryActions(false);
		toggleModificationActions(false);
		togglePasteAction(false);
		d->tasksWidget->toggleRunNowAction(false);
	}
	else {
		logDebug() << "Enabling view..." << endl;

		d->tasksWidget->treeWidget()->setEnabled(true);
		d->variablesWidget->treeWidget()->setEnabled(true);

		toggleNewEntryActions(true);
		togglePasteAction(hasClipboardContent());
	}
}


void CrontabWidget::copy() {
	foreach(CTTask* task, d->clipboardTasks) {
		delete task;
	}
	d->clipboardTasks.clear();

	foreach(CTVariable* variable, d->clipboardVariables) {
		delete variable;
	}
	d->clipboardVariables.clear();

	QString clipboardText;

	if (d->tasksWidget->treeWidget()->hasFocus()) {
		logDebug() << "Tasks copying" << endl;

		QList<TaskWidget*> tasksWidget = d->tasksWidget->selectedTasksWidget();
		foreach(TaskWidget* taskWidget, tasksWidget) {
			CTTask* task = new CTTask( *(taskWidget->getCTTask()) );
			d->clipboardTasks.append(task);

			clipboardText += task->exportTask() + QLatin1String( "\n" );
		}
	}

	if (d->variablesWidget->treeWidget()->hasFocus()) {
		logDebug() << "Variables copying" << endl;

		QList<VariableWidget*> variablesWidget = d->variablesWidget->selectedVariablesWidget();
		foreach(VariableWidget* variableWidget, variablesWidget) {
			CTVariable* variable = new CTVariable( *(variableWidget->getCTVariable()) );
			d->clipboardVariables.append(variable);

			clipboardText += variable->exportVariable() + QLatin1String( "\n" );
		}
	}

	QApplication::clipboard()->setText(clipboardText, QClipboard::Clipboard);
	QApplication::clipboard()->setText(clipboardText, QClipboard::Selection);

	logDebug() << "Clipboard Status " << hasClipboardContent() << endl;
	togglePasteAction(hasClipboardContent());
}

void CrontabWidget::cut() {
	logDebug() << "Cut content" << endl;

	copy();

	if (d->tasksWidget->treeWidget()->hasFocus()) {
		logDebug() << "Tasks cutting" << endl;
		d->tasksWidget->deleteSelection();
	}

	if (d->variablesWidget->treeWidget()->hasFocus()) {
		logDebug() << "Variables cutting" << endl;
		d->variablesWidget->deleteSelection();
	}
}

void CrontabWidget::paste() {
	logDebug() << "Paste content" << endl;

	if (d->tasksWidget->treeWidget()->hasFocus()) {
		foreach(CTTask* task, d->clipboardTasks) {
			d->tasksWidget->addTask(new CTTask(*task));
		}
	}

	if (d->variablesWidget->treeWidget()->hasFocus()) {
		foreach(CTVariable* variable, d->clipboardVariables) {
			d->variablesWidget->addVariable(new CTVariable(*variable));
		}
	}

}

CTCron* CrontabWidget::currentCron() const {
	if (d->currentUserCronRadio->isChecked())
		return d->ctHost->findCurrentUserCron();
	else if (d->systemCronRadio->isChecked())
		return d->ctHost->findSystemCron();

	if (d->otherUsers->currentIndex() == d->otherUsers->count()-1) {
		logDebug() << "Using Global Cron" << endl;
		return d->ctGlobalCron;
	}

	QString currentUserLogin = d->otherUsers->currentText();
	return d->ctHost->findUserCron(currentUserLogin);
}

TasksWidget* CrontabWidget::tasksWidget() const {
	return d->tasksWidget;
}

VariablesWidget* CrontabWidget::variablesWidget() const {
	return d->variablesWidget;
}

CTHost* CrontabWidget::ctHost() const {
	return d->ctHost;
}

void CrontabWidget::checkOtherUsers() {
	d->otherUserCronRadio->setChecked(true);

	refreshCron();
}

void CrontabWidget::setupActions() {
	logDebug() << "Setup actions" << endl;

	//Edit menu
	d->cutAction = KStandardAction::cut(this, SLOT(cut()), this);
	d->copyAction = KStandardAction::copy(this, SLOT(copy()), this);
	d->pasteAction = KStandardAction::paste(this, SLOT(paste()), this);
	togglePasteAction(false);

	logDebug() << "Actions initialized" << endl;

}


QList<QAction*> CrontabWidget::cutCopyPasteActions() {
	QList<QAction*> actions;
	actions.append(d->cutAction);
	actions.append(d->copyAction);
	actions.append(d->pasteAction);

	return actions;
}

void CrontabWidget::togglePasteAction(bool state) {
	d->pasteAction->setEnabled(state);
}

void CrontabWidget::toggleModificationActions(bool state) {
	d->cutAction->setEnabled(state);
	d->copyAction->setEnabled(state);

	d->tasksWidget->toggleModificationActions(state);
	d->variablesWidget->toggleModificationActions(state);
}

void CrontabWidget::toggleNewEntryActions(bool state) {
	d->tasksWidget->toggleNewEntryAction(state);
	d->variablesWidget->toggleNewEntryAction(state);
}

void CrontabWidget::print() {

	CrontabPrinter printer(this);

	if (printer.start() == false) {
		logDebug() << "Unable to start printer" << endl;
		return;
	}
	printer.printTasks();
	printer.printVariables();

	printer.finish();

}


