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
#include <QDateTime>
#include <QLabel>
#include <QSplitter>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QApplication>
#include <QClipboard>

#include <kglobalsettings.h>
#include <klocale.h>
#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"
#include "ctGlobalCron.h"

#include "kcronIcons.h"
#include "tasksWidget.h"
#include "taskWidget.h"

#include "variableWidget.h"
#include "variableWidget.h"

#include "kcmCron.h"
#include "crontabPrinter.h"

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
	d->ctGlobalCron = new CTGlobalCron(d->ctHost);

	initialize();

	logDebug() << "Clipboard Status " << hasClipboardContent() << endl;
	emit modificationActionsToggled(false);
	emit runNowActionToggled(false);
	
	d->tasksWidget->setFocus();

	QTreeWidgetItem* item = d->tasksWidget->treeWidget()->topLevelItem(0);
	if (item != NULL) {
		logDebug() << "First item found" << d->tasksWidget->treeWidget()->topLevelItemCount() << endl;
		item->setSelected(true);
	}

}

bool CrontabWidget::hasClipboardContent() {
	if (d->clipboardTasks.isEmpty() == false)
		return true;

	if (d->clipboardVariables.isEmpty() == false)
		return true;

	return false;
}

QHBoxLayout* CrontabWidget::createCronSelector() {
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setSpacing(4);

	QLabel* cronSelectorIcon = new QLabel(this);
	cronSelectorIcon->setPixmap(SmallIcon("table"));
	layout->addWidget(cronSelectorIcon);

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
		d->otherUsers->addItem(KIcon("users"), i18n("Show All Personal Crons"));
	} else {
		d->otherUserCronRadio->hide();
		d->otherUsers->hide();
	}

	connect(group, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(refreshCron()));
	connect(d->otherUsers, SIGNAL(currentIndexChanged(int)), this, SLOT(checkOtherUsers()));

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

	logDebug() << "Variables list created" << endl;

	connect(d->tasksWidget->treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(slotSetCurrentItem()));
	connect(d->variablesWidget->treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(slotSetCurrentItem()));

	logDebug() << "View initialized" << endl;

	refreshCron();

}

CrontabWidget::~CrontabWidget() {
	delete d->tasksWidget;
	delete d->variablesWidget;

	delete d;
}

void CrontabWidget::refreshCron() {

	CTCron* ctCron = currentCron();

	d->tasksWidget->refreshTasks(ctCron);
	d->variablesWidget->refreshVariables(ctCron);

	if (ctCron->isMultiUserCron() && ctHost()->isRootUser()==false) {
		d->tasksWidget->setEnabled(false);
		d->variablesWidget->setEnabled(false);

		emit newEntryActionsToggled(false);
		emit modificationActionsToggled(false);
		emit runNowActionToggled(false);
		emit pasteActionToggled(false);
	}
	else {
		d->tasksWidget->setEnabled(true);
		d->variablesWidget->setEnabled(true);

		emit newEntryActionsToggled(true);
		emit pasteActionToggled(hasClipboardContent());
	}
}

void CrontabWidget::slotSetCurrentItem() {
	if (d->tasksWidget->treeWidget()->hasFocus()) {
		QList<QTreeWidgetItem*> variablesItems = d->variablesWidget->treeWidget()->selectedItems();
		foreach(QTreeWidgetItem* item, variablesItems) {
			item->setSelected(false);
		}

	} else {
		QList<QTreeWidgetItem*> tasksItems = d->tasksWidget->treeWidget()->selectedItems();
		foreach(QTreeWidgetItem* item, tasksItems) {
			item->setSelected(false);
		}

	}

	TaskWidget* taskWidget = d->tasksWidget->firstSelectedTaskWidget();
	VariableWidget* variableWidget = d->variablesWidget->firstSelectedVariableWidget();

	if (taskWidget!=NULL || variableWidget!=NULL) {
		emit modificationActionsToggled(true);
	}

	if (taskWidget==NULL)
		emit runNowActionToggled(false);
	else
		emit runNowActionToggled(true);

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
			
			clipboardText += task->exportTask() + "\n";
		}
	}

	if (d->variablesWidget->treeWidget()->hasFocus()) {
		logDebug() << "Variables copying" << endl;
		
		QList<VariableWidget*> variablesWidget = d->variablesWidget->selectedVariablesWidget();
		foreach(VariableWidget* variableWidget, variablesWidget) {
			CTVariable* variable = new CTVariable( *(variableWidget->getCTVariable()) );
			d->clipboardVariables.append(variable);
			
			clipboardText += variable->exportVariable() + "\n";
		}
	}
	
	QApplication::clipboard()->setText(clipboardText, QClipboard::Clipboard);
	QApplication::clipboard()->setText(clipboardText, QClipboard::Selection);

	logDebug() << "Clipboard Status " << hasClipboardContent() << endl;
	emit pasteActionToggled(hasClipboardContent());
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

#include "crontabWidget.moc"
