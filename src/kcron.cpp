/***************************************************************************
 *   KT application implementation.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kcron.h"

#include <QLabel>

#include <kmenubar.h>
#include <kstandardshortcut.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstatusbar.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include <kxmlguifactory.h>
#include <kglobal.h>
#include <ktoolbar.h>
#include <kicon.h>
#include <kconfiggroup.h>

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"

#include "kcronIcons.h"
#include "crontabWidget.h"
#include "logging.h"

class KCronPrivate {
public:

	/**
	 * Configuration object of the application.
	 */
	KSharedConfigPtr config;

	/**
	 * Main GUI view/working area.
	 */
	CrontabWidget* crontabWidget;

	/**
	 * Document object, here crotab enries.
	 */
	CTHost* ctHost;

	/**
	 * Path to the crontab binary
	 */
	QString crontab;

	QAction* cutAction;
	QAction* copyAction;
	QAction* pasteAction;

	QAction* newTaskAction;

	QAction* newVariableAction;

	QAction* modifyAction;

	QAction* deleteAction;

	QAction* runNowAction;
};

KCron::KCron() :
	KXmlGuiWindow(NULL), d(new KCronPrivate()) {

	d->config = KGlobal::config();

	setWindowIcon(KCronIcons::application(KCronIcons::Small));

	setCaption(i18n("Task Scheduler"));

	// Read options.
	readOptions();

	// Initialize document.
	d->ctHost = new CTHost(d->crontab);
	d->crontabWidget = new CrontabWidget(this, d->ctHost);

	connect(d->crontabWidget, SIGNAL(modificationActionsToggled(bool)), this, SLOT(toggleModificationActions(bool)));
	connect(d->crontabWidget, SIGNAL(pasteActionToggled(bool)), this, SLOT(togglePasteAction(bool)));
	connect(d->crontabWidget, SIGNAL(runNowActionToggled(bool)), this, SLOT(toggleRunNowActions(bool)));
	connect(d->crontabWidget, SIGNAL(newEntryActionsToggled(bool)), this, SLOT(toggleNewEntryActions(bool)));
	

	// Call inits to invoke all other construction parts.
	setupActions();
	initStatusBar();

	setupGUI(QSize(700, 500));

	// Initialize view.
	setCentralWidget(d->crontabWidget);

}

bool KCron::init() {
	if (d->ctHost->isError()) {
		KMessageBox::error(this, i18n("The following error occurred while initializing KCron:"
			"\n\n%1\n\nKCron will now exit.\n", d->ctHost->errorMessage()));
		return false;
	}

	// Display greeting screen.
	// if there currently are no scheduled tasks...
	if (!d->ctHost->isRootUser()) {
		int taskCount = 0;
		foreach(CTCron* ctCron, d->ctHost->crons) {
			taskCount += ctCron->tasks().count();
		}

		if (taskCount == 0) {
			show();
			KMessageBox::information(this, i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on the Tasks folder and select Edit/New from the menu."), i18n("Welcome to the Task Scheduler"), "welcome");
		}
	}
	return true;
}

KCron::~KCron() {
	delete d->crontabWidget;
	delete d->ctHost;

	delete d;
}

CTHost* KCron::getCTHost() const {
	return d->ctHost;
}

QString KCron::caption() {
	return KGlobal::caption();
}

void KCron::setupActions() {
	//File Menu
	KStandardAction::save(this, SLOT(slotSave()), actionCollection());
	KStandardAction::quit(this, SLOT(slotQuit()), actionCollection());
	KStandardAction::print(d->crontabWidget, SLOT(print()), actionCollection());

	//Edit menu
	d->cutAction = KStandardAction::cut(d->crontabWidget, SLOT(cut()), actionCollection());
	d->copyAction = KStandardAction::copy(d->crontabWidget, SLOT(copy()), actionCollection());
	d->pasteAction = KStandardAction::paste(d->crontabWidget, SLOT(paste()), actionCollection());
	togglePasteAction(false);
	

	d->newTaskAction = actionCollection()->addAction("edit_new_task");
	d->newTaskAction->setIcon(KIcon("document-new"));
	d->newTaskAction->setText(i18nc("Adds a new task", "New &Task...") );
	connect(d->newTaskAction, SIGNAL(triggered(bool)), d->crontabWidget->tasksWidget(), SLOT(createTask()));

	d->newVariableAction = actionCollection()->addAction("edit_new_variable");
	d->newVariableAction->setIcon(KIcon("document-new"));
	d->newVariableAction->setText(i18nc("Adds a new variable", "New &Variable...") );
	connect(d->newVariableAction, SIGNAL(triggered(bool)), d->crontabWidget->variablesWidget(), SLOT(createVariable()));

	d->modifyAction = actionCollection()->addAction("edit_modify");
	d->modifyAction->setText(i18n("M&odify...") );
	d->modifyAction->setIcon(KIcon("document-open") );
	connect(d->modifyAction, SIGNAL(triggered(bool)), d->crontabWidget->tasksWidget(), SLOT(modifySelection()));
	connect(d->modifyAction, SIGNAL(triggered(bool)), d->crontabWidget->variablesWidget(), SLOT(modifySelection()));

	d->deleteAction = actionCollection()->addAction("edit_delete");
	d->deleteAction->setText(i18n("&Delete") );
	d->deleteAction->setIcon(KIcon("edit-delete") );
	connect(d->deleteAction, SIGNAL(triggered(bool)), d->crontabWidget->tasksWidget(), SLOT(deleteSelection()));
	connect(d->deleteAction, SIGNAL(triggered(bool)), d->crontabWidget->variablesWidget(), SLOT(deleteSelection()));

	d->runNowAction = actionCollection()->addAction("edit_run");
	d->runNowAction->setText(i18n("&Run Now") );
	d->runNowAction->setIcon(KIcon("system-run"));
	connect(d->runNowAction, SIGNAL(triggered(bool)), d->crontabWidget->tasksWidget(), SLOT(runTaskNow()));

	//TODO Enable this when the bug in actionHighlighted of KActionCollection will be fixed.
	//connect(actionCollection(), SIGNAL(actionHighlighted(QAction*)), this, SLOT(displayActionInformation(QAction*)));
	
	prepareTasksWidgetContextualMenu();
	prepareVariablesWidgetContextualMenu();

}

void KCron::prepareTasksWidgetContextualMenu() {

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->newTaskAction);

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->modifyAction);
	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->deleteAction);

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->cutAction);
	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->copyAction);
	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->pasteAction);

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->tasksWidget()->treeWidget()->addAction(d->runNowAction);
}

void KCron::prepareVariablesWidgetContextualMenu() {

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->newVariableAction);

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->modifyAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->deleteAction);

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->cutAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->copyAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->pasteAction);

}

QAction* KCron::createSeparator() {
	QAction* action = new QAction(this);
	action->setSeparator(true);

	return action;
}

void KCron::initStatusBar() {
	QLabel* defaultMessage = new QLabel(i18nc("Ready for user input", " Ready."));
	defaultMessage->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	statusBar()->addWidget(defaultMessage);
}

void KCron::saveOptions() {
	KConfigGroup group(d->config, "General Options");
	group.writeEntry(QString("Path to crontab"), d->crontab);
}

void KCron::readOptions() {
	KConfigGroup group(d->config, "General Options");

	// get the path to the crontab binary
	d->crontab = group.readEntry(QString("Path to crontab"), QString("crontab"));
}

bool KCron::queryClose() {
	if (d->ctHost->isDirty()) {
		KCron* win = (KCron*)parent();

		int retVal = KMessageBox::warningYesNoCancel(win, i18n("Scheduled tasks have been modified.\nDo you want to save changes?"), QString(), KStandardGuiItem::save(), KStandardGuiItem::discard() );

		switch (retVal) {
		case KMessageBox::Yes:
			d->ctHost->save();
			if (d->ctHost->isError()) {
				KMessageBox::error(win, d->ctHost->errorMessage());
				return false;
			}
			return true;
			break;
		case KMessageBox::No:
			return true;
			break;
		case KMessageBox::Cancel:
			return false;
			break;
		default:
			return false;
			break;
		}
	} else {
		return true;
	}
}

bool KCron::queryExit() {
	saveOptions();
	return true;
}

void KCron::slotSave() {
	slotStatusMessage(i18nc("Saving the file to the hard drive", "Saving..."));
	d->ctHost->save();
	slotStatusMessage(i18nc("Ready for user input", "Ready."));
	if (d->ctHost->isError()) {
		KMessageBox::error(this, d->ctHost->errorMessage());
	}
}

void KCron::slotQuit() {
	saveOptions();
	close();
}

void KCron::slotStatusMessage(const QString & text) {
	statusBar()->showMessage(text);
	setCaption(i18n("Task Scheduler"), d->ctHost->isDirty());
}

void KCron::slotStatusHelpMessage(const QString & text) {
	statusBar()->showMessage(text, 2000);
}

void KCron::displayActionInformation(QAction* action) {
	
	if (action == d->newTaskAction) {
		slotStatusHelpMessage(i18n("Create a new task."));
	}
	else if (action == d->newVariableAction) {
		slotStatusHelpMessage(i18n("Create a new variable."));
	}
	else if (action == d->modifyAction) {
		slotStatusHelpMessage(i18n("Edit the selected task or variable."));
	}
	else if (action == d->deleteAction) {
		slotStatusHelpMessage(i18n("Delete the selected task or variable."));
	}
	else if (action == d->runNowAction) {
		slotStatusHelpMessage(i18n("Run the selected task now."));
	}
}

void KCron::toggleModificationActions(bool state) {
	d->cutAction->setEnabled(state);
	d->copyAction->setEnabled(state);
	d->modifyAction->setEnabled(state);
	d->deleteAction->setEnabled(state);

}

void KCron::togglePasteAction(bool state) {
	logDebug() << "Set paste enabled " << state << endl;
	d->pasteAction->setEnabled(state);
}

void KCron::toggleRunNowActions(bool state) {
	d->runNowAction->setEnabled(state);
}

void KCron::toggleNewEntryActions(bool state) {
	d->newTaskAction->setEnabled(state);
	d->newVariableAction->setEnabled(state);
}

#include "kcron.moc"
