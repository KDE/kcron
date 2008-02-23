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
#include <QDateTime>
#include <QResizeEvent>
#include <QLabel>
#include <QSplitter>

#include <kglobalsettings.h>
#include <klocale.h>
#include <kglobal.h>

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
#include "ktprint.h"

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

};

CrontabWidget::CrontabWidget(QWidget* parent, CTHost* ctHost) :
	QWidget(parent), 
	d(new CrontabWidgetPrivate()) {

	d->tasksWidget = NULL;
	d->variablesWidget = NULL;
	/*
	d->clipboardIsTask = true;
	d->clipboardCTTask = NULL;
	d->clipboardCTVariable = NULL;
	*/
	d->ctHost = ctHost;

	initialize();

	emit modificationActionsToggled(false);
	emit runNowActionToggled(false);
	//emit pasteActionToggled(d->clipboardCTVariable);

	d->tasksWidget->setFocus();
	
	QTreeWidgetItem* item = d->tasksWidget->treeWidget()->topLevelItem(0);
	if (item != NULL) {
		logDebug() << "First item found" << d->tasksWidget->treeWidget()->topLevelItemCount() << endl;
		item->setSelected(true);
	}


}

void CrontabWidget::initialize() {
	QVBoxLayout* layout = new QVBoxLayout(this);

	logDebug() << "Begin view refresh" << endl;

	logDebug() << "Creating Tasks list..." << endl;

	QSplitter* splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Vertical);
	layout->addWidget(splitter);

	d->tasksWidget = new TasksWidget(this, d->ctHost);
	splitter->addWidget(d->tasksWidget);
	splitter->setStretchFactor(0, 2);

	d->variablesWidget = new VariablesWidget(this, d->ctHost);
	splitter->addWidget(d->variablesWidget);
	splitter->setStretchFactor(1, 1);

	logDebug() << "Variables list created" << endl;

	// For each user
	foreach(CTCron* ctCron, d->ctHost->cron) {

		foreach(CTTask* ctTask, ctCron->task) {
			new TaskWidget(d->tasksWidget, ctTask);
		}

		foreach(CTVariable* ctVariable, ctCron->variable) {
			new VariableWidget(d->variablesWidget, ctVariable);
		}
	}

	//Resize all columns except the last one (which always take the last available space)
	d->tasksWidget->resizeColumnContents();
	d->variablesWidget->resizeColumnContents();

	logDebug() << "Connecting, showing" << endl;

	connect(d->tasksWidget->treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(slotSetCurrentItem()));
	connect(d->variablesWidget->treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(slotSetCurrentItem()));

	logDebug() << "View refreshed" << endl;

}

CrontabWidget::~CrontabWidget() {
	delete d->tasksWidget;
	delete d->variablesWidget;
	
	delete d;
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
	/*
	 bool crontab, allUsers;
	 KTListItem *ktli, *user;

	 const CTHost& cth(ktapp->getCTHost());

	 KTPrint printer(cth.isRootUser(), this);

	 if (printer.start()) {
	 crontab = printer.crontab();
	 allUsers = printer.allUsers();

	 printer.createColumns(3);

	 int copies = printer.numCopies();
	 while (copies != 0) {
	 if (allUsers || !cth.isRootUser()) {
	 ktli = (KTListItem*)listView->firstChild();
	 } else {
	 ktli = (KTListItem*)listView->currentItem();
	 }

	 //Check that the correct item is selected, they must
	 //select the top level entry for all items to print
	 while (ktli->depth() != 0)
	 ktli = (KTListItem*)ktli->parent();

	 user = ktli; //Used to store user's name

	 if (allUsers) {
	 while (ktli) {
	 pageHeading(ktli, printer);
	 ktli->print(printer);
	 if (crontab)
	 pageFooter(ktli, printer);
	 ktli = (KTListItem*)ktli->nextSibling();
	 if (ktli)
	 printer.newPage();
	 }
	 } else {
	 //ktli goes out of range here hence the need for user
	 pageHeading(user, printer);
	 if (!cth.isRootUser()) {
	 while (ktli) {
	 ktli->print(printer);
	 ktli = (KTListItem*)ktli->nextSibling();
	 }
	 } else
	 ktli->print(printer);

	 if (crontab)
	 pageFooter(user, printer);

	 }

	 copies--; //Keep a track of how many copies we have printed
	 if (copies != 0)
	 printer.newPage();
	 }//End printing loop (for more than one copy)

	 printer.finished(); //End the print

	 }//End Printing if
	 */
}

void CrontabWidget::pageHeading(/*KTListItem* user, */KTPrint& /*printer*/) const {
	/*
	 QFont stnd;
	 QString logonInfo;
	 QDateTime now(QDateTime::currentDateTime());
	 char hostName[20];

	 gethostname(hostName, 20);
	 // SSA : Fix Me user name, logon name and host name must be
	 // SSA : not only in us-ascii ??
	 logonInfo = i18nc("user on host", "%1 <placeholder>%2</placeholder> on %3", user->getCTCron()->name, user->getCTCron()->login, QString::fromLocal8Bit(hostName));

	 stnd = printer.getFont();
	 printer.setFont(QFont(KGlobalSettings::generalFont().family(), 14, QFont::Bold));

	 printer.print(i18n("Scheduled Tasks"), 2, KTPrint::alignTextCenter, false);
	 printer.print(logonInfo, 2, KTPrint::alignTextCenter, false);
	 printer.print(KGlobal::locale()->formatDateTime(now), 2, KTPrint::alignTextCenter, false);
	 printer.setFont(stnd);

	 printer.levelColumns(20);
	 */

}

void CrontabWidget::pageFooter(/*KTListItem* user, */KTPrint& /*printer*/) const {
	/*
	 QString crontab = user->getCTCron()->exportCron();
	 printer.print(crontab, 1, KTPrint::alignTextLeft, false);
	 */
}

TasksWidget* CrontabWidget::tasksWidget() {
	return d->tasksWidget;
}

VariablesWidget* CrontabWidget::variablesWidget() {
	return d->variablesWidget;
}

#include "crontabWidget.moc"
