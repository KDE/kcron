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

#include <kglobalsettings.h>
#include <klocale.h>
#include <kglobal.h>
#include <kicon.h>
#include <kiconloader.h>

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"

#include "kticon.h"
#include "tasksWidget.h"
#include "taskWidget.h"

#include "variableWidget.h"
#include "variableWidget.h"

#include "kcron.h"
#include "crontabPrinter.h"

#include "logging.h"

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
	 * Indicates whether or not the item on the clipboard is a task.
	 */
	//bool clipboardIsTask;

	/**
	 * Clipboard task.
	 */
	//CTTask* clipboardCTTask;

	/**
	 * Clipboard variable.
	 */
	//CTVariable* clipboardCTVariable;

	QRadioButton* currentUserCronRadio;
	QRadioButton* systemCronRadio;
	QRadioButton* otherUserCronRadio;

	QComboBox* otherUsers;

};

CrontabWidget::CrontabWidget(QWidget* parent, CTHost* ctHost) :
	QWidget(parent), d(new CrontabWidgetPrivate()) {

	d->tasksWidget = NULL;
	d->variablesWidget = NULL;

	/*
	 d->clipboardIsTask = true;
	 d->clipboardCTTask = NULL;
	 d->clipboardCTVariable = NULL;
	 */
	d->ctHost = ctHost;

	initialize();

	emit 	modificationActionsToggled(false);
	emit 	runNowActionToggled(false);
	//emit pasteActionToggled(d->clipboardCTVariable);

	d->tasksWidget->setFocus();

	QTreeWidgetItem* item = d->tasksWidget->treeWidget()->topLevelItem(0);
	if (item != NULL) {
		logDebug() << "First item found" << d->tasksWidget->treeWidget()->topLevelItemCount() << endl;
		item->setSelected(true);
	}

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

		foreach(CTCron* ctCron, ctHost()->cron) {
			d->otherUsers->addItem(ctCron->userLogin());

		}

		//d->otherUsers->addItem(KIcon("users"), i18n("All users"));
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
	
	if (ctCron->isSystemCron() && ctHost()->isRootUser()==false) {
		d->tasksWidget->setEnabled(false);
		d->variablesWidget->setEnabled(false);
		
		emit newEntryToggled(false);
		emit modificationActionsToggled(false);
		emit runNowActionToggled(false);
	}
	else {
		d->tasksWidget->setEnabled(true);
		d->variablesWidget->setEnabled(true);
		
		emit newEntryToggled(true);
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

	CrontabPrinter printer(d->ctHost->isRootUser(), this);

	if (printer.start() == false) {
		logDebug() << "Unable to start printer" << endl;
		return;
	}

	printer.createColumns(3);

	int copies = printer.numCopies();
	while (copies != 0) {
		pageHeading(printer);
		
		tasksWidget()->print(printer);
		variablesWidget()->print(printer);

		if (printer.crontab()) {
			pageFooter(printer);
		}


		copies--;
		if (copies != 0)
			printer.newPage();
	

	}

	printer.finished(); //End the print
	
}

void CrontabWidget::pageHeading(CrontabPrinter& printer) const {
	QDateTime now(QDateTime::currentDateTime());
	char hostName[20];

	gethostname(hostName, 20);
	// SSA : Fix Me user name, logon name and host name must be
	// SSA : not only in us-ascii ??
	QString logonInfo = i18nc("user on host", "%1 <placeholder>%2</placeholder> on %3", currentCron()->userRealName(), currentCron()->userLogin(), QString::fromLocal8Bit(hostName));

	QFont stnd = printer.getFont();
	printer.setFont(QFont(KGlobalSettings::generalFont().family(), 14, QFont::Bold));

	printer.print(i18n("Scheduled Tasks"), 2, CrontabPrinter::alignTextCenter, false);
	printer.print(logonInfo, 2, CrontabPrinter::alignTextCenter, false);
	printer.print(KGlobal::locale()->formatDateTime(now), 2, CrontabPrinter::alignTextCenter, false);
	printer.setFont(stnd);

	printer.levelColumns(20);

}

void CrontabWidget::pageFooter(CrontabPrinter& printer) const {
	QString crontab = currentCron()->exportCron();
	printer.print(crontab, 1, CrontabPrinter::alignTextLeft, false);
}

void CrontabWidget::copy() {
	/*
	 if (d->clipboardCTTask) {
	 delete d->clipboardCTTask;
	 d->clipboardCTTask = NULL;
	 }

	 if (d->clipboardCTVariable) {
	 delete d->clipboardCTVariable;
	 d->clipboardCTVariable = NULL;
	 }

	 TaskWidget* taskWidget = d->tasksWidget->firstSelectedTaskWidget();
	 VariableWidget* variableWidget = d->variablesWidget->firstSelectedVariableWidget();

	 if (taskWidget != NULL) {
	 d->clipboardCTTask = new CTTask( *(taskWidget->getCTTask()) );
	 d->clipboardIsTask = true;
	 } else if (variableWidget != NULL) {
	 d->clipboardCTVariable = new CTVariable( *(variableWidget->getCTVariable()) );
	 d->clipboardIsTask = false;
	 }
	 */
}

void CrontabWidget::cut() {
	/*
	 copy();
	 
	 d->tasksWidget->deleteSelection();
	 d->variablesWidget->deleteSelection();
	 */
}

void CrontabWidget::paste() {
	/*
	 KTListItem* qlvi = (KTListItem*)tasksListWidget->currentItem();

	 if (currentIsTask) {
	 CTTask* temptask = new CTTask(*clipboardCTTask);
	 currentCTCron->task.push_back(temptask);
	 KTListTask* ktlt = new KTListTask(qlvi, currentCTCron, temptask);
	 tasksListWidget->setSelected(ktlt, true);
	 } else {
	 CTVariable* tempvar = new CTVariable(*clipboardCTVariable);
	 currentCTCron->variable.push_back(tempvar);
	 KTListVar* ktlv = new KTListVar(qlvi, currentCTCron, tempvar);
	 tasksListWidget->setSelected(ktlv, true);
	 }
	 */
}

CTCron* CrontabWidget::currentCron() const {
	if (d->currentUserCronRadio->isChecked())
		return d->ctHost->findCurrentUserCron();
	else if (d->systemCronRadio->isChecked())
		return d->ctHost->findSystemCron();

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
