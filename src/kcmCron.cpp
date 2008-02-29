/***************************************************************************
 *   KT icons implementation.                                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "kcmCron.h"

#include <QImage>
#include <QPixmap>
#include <QLayout>

#include <QComboBox>

#include <QLabel>

#include <kglobal.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>

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

#include "kcronIcons.h"
#include "crontabWidget.h"

#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"

#include "logging.h"

typedef KGenericFactory<KCMCron, QWidget> KCMCronFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_cron, KCMCronFactory("kcron.po") )

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

	KActionCollection* actionCollection;
	
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



KCMCron::KCMCron(QWidget* parent, const QStringList& /*args*/) : 
	KCModule(KCMCronFactory::componentData(), parent),
	d(new KCronPrivate()) {

	logDebug() << "KCMCRON CONSTRUCTOR !!!!!!!!!!!!!" << endl;
	
	KAboutData* aboutData = new KAboutData("kcm_cron", 0, ki18n("Task Scheduler"),
			KDE_VERSION_STRING, ki18n("KDE Task Scheduler"), KAboutData::License_GPL, ki18n("(c) 2008, Nicolas Ternisien\n(c) 1999-2000, Gary Meyer"));

	aboutData->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");
	aboutData->addAuthor(ki18n("Gary Meyer"), KLocalizedString(), "gary@meyer.net");
	aboutData->addAuthor(ki18n("Robert Berry"), KLocalizedString(), "rjmber@ntlworld.com");
	aboutData->addAuthor(ki18n("James Ots"), KLocalizedString(), "code@jamesots.com");
	aboutData->addAuthor(ki18n("Alberto G. Hierro"), KLocalizedString(), "alberto.hierro@kdemail.net");

	setAboutData(aboutData);

	d->actionCollection = new KActionCollection(this, KCMCronFactory::componentData());
	
	d->config = KGlobal::config();

	//setWindowIcon(KCronIcons::application(KCronIcons::Small));

	//setCaption(i18n("Task Scheduler"));

	// Read options.
	readOptions();
	

	// Initialize document.
	d->ctHost = new CTHost(d->crontab);
	d->crontabWidget = new CrontabWidget(this, d->ctHost);
	
	logDebug() << "Crontab Widget initialized" << endl;

	connect(d->crontabWidget, SIGNAL(modificationActionsToggled(bool)), this, SLOT(toggleModificationActions(bool)));
	connect(d->crontabWidget, SIGNAL(pasteActionToggled(bool)), this, SLOT(togglePasteAction(bool)));
	connect(d->crontabWidget, SIGNAL(runNowActionToggled(bool)), this, SLOT(toggleRunNowActions(bool)));
	connect(d->crontabWidget, SIGNAL(newEntryActionsToggled(bool)), this, SLOT(toggleNewEntryActions(bool)));
	

	// Call inits to invoke all other construction parts.
	setupActions();
	initStatusBar();

	// Initialize view.
	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);
	
	layout->addWidget(d->crontabWidget);
	
	init();

}

KCMCron::~KCMCron() {
	logDebug() << "KCMCRON DESTRUCTOR !!!!!!!!!!!!!" << endl;
	
	delete d->crontabWidget;
	delete d->ctHost;

	delete d;
}

void KCMCron::load() {
	logDebug() << "KCMCRON load !!!!!!!!!!!!!" << endl;

}

void KCMCron::save() {
	logDebug() << "KCMCRON save !!!!!!!!!!!!!" << endl;
		
}

void KCMCron::defaults() {
	logDebug() << "KCMCRON defaults !!!!!!!!!!!!!" << endl;
	
}


KActionCollection* KCMCron::actionCollection() {
	return d->actionCollection;
}

bool KCMCron::init() {
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

CTHost* KCMCron::getCTHost() const {
	return d->ctHost;
}

QString KCMCron::caption() {
	return KGlobal::caption();
}

void KCMCron::setupActions() {
	logDebug() << "Setup actions" << endl;

	/**

<!DOCTYPE kpartgui SYSTEM "kpartgui.dtd">
<kpartgui name="kcron" version="1">
<MenuBar>
  <Menu name="edit">
    <Separator/>
    <Action name="edit_new_task" />
    <Action name="edit_new_variable" />
    <Separator/>
    <Action name="edit_modify" />
    <Action name="edit_delete" />
    <Separator/>
    <Action name="edit_run" />
  </Menu>
  <Menu name="settings">
    <Action name="show_toolbar" />
    <Action name="show_statusbar" />
  </Menu>
</MenuBar>

<ToolBar name="mainToolBar">
  <Action name="edit_new_task" />
  <Action name="edit_new_variable" />
  <Separator/>
  <Action name="edit_modify" />
  <Action name="edit_delete" />
</ToolBar>

</kpartgui>


	*/
	
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
	
	logDebug() << "Actions initialized" << endl;

}

void KCMCron::prepareTasksWidgetContextualMenu() {

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

void KCMCron::prepareVariablesWidgetContextualMenu() {

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->newVariableAction);

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->modifyAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->deleteAction);

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(createSeparator());

	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->cutAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->copyAction);
	d->crontabWidget->variablesWidget()->treeWidget()->addAction(d->pasteAction);

}

QAction* KCMCron::createSeparator() {
	QAction* action = new QAction(this);
	action->setSeparator(true);

	return action;
}

void KCMCron::initStatusBar() {
	/*
	QLabel* defaultMessage = new QLabel(i18nc("Ready for user input", " Ready."));
	defaultMessage->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	statusBar()->addWidget(defaultMessage);
	*/
}

void KCMCron::saveOptions() {
	KConfigGroup group(d->config, "General Options");
	group.writeEntry(QString("Path to crontab"), d->crontab);
}

void KCMCron::readOptions() {
	KConfigGroup group(d->config, "General Options");

	// get the path to the crontab binary
	d->crontab = group.readEntry(QString("Path to crontab"), QString("crontab"));
}

bool KCMCron::queryClose() {
	if (d->ctHost->isDirty()) {
		int retVal = KMessageBox::warningYesNoCancel(this, i18n("Scheduled tasks have been modified.\nDo you want to save changes?"), QString(), KStandardGuiItem::save(), KStandardGuiItem::discard() );

		switch (retVal) {
		case KMessageBox::Yes:
			d->ctHost->save();
			if (d->ctHost->isError()) {
				KMessageBox::error(this, d->ctHost->errorMessage());
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

bool KCMCron::queryExit() {
	saveOptions();
	return true;
}

void KCMCron::slotSave() {
	slotStatusMessage(i18nc("Saving the file to the hard drive", "Saving..."));
	d->ctHost->save();
	slotStatusMessage(i18nc("Ready for user input", "Ready."));
	if (d->ctHost->isError()) {
		KMessageBox::error(this, d->ctHost->errorMessage());
	}
}

void KCMCron::slotQuit() {
	saveOptions();
	close();
}

void KCMCron::slotStatusMessage(const QString& /*text*/) {
	//statusBar()->showMessage(text);
	//setCaption(i18n("Task Scheduler"), d->ctHost->isDirty());
}

void KCMCron::slotStatusHelpMessage(const QString& /*text*/) {
	//statusBar()->showMessage(text, 2000);
}

void KCMCron::displayActionInformation(QAction* action) {
	
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

void KCMCron::toggleModificationActions(bool state) {
	d->cutAction->setEnabled(state);
	d->copyAction->setEnabled(state);
	d->modifyAction->setEnabled(state);
	d->deleteAction->setEnabled(state);

}

void KCMCron::togglePasteAction(bool state) {
	logDebug() << "Set paste enabled " << state << endl;
	d->pasteAction->setEnabled(state);
}

void KCMCron::toggleRunNowActions(bool state) {
	d->runNowAction->setEnabled(state);
}

void KCMCron::toggleNewEntryActions(bool state) {
	d->newTaskAction->setEnabled(state);
	d->newVariableAction->setEnabled(state);
}

#include "kcmCron.moc"
