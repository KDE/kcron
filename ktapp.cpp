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

#include "ktapp.h"
#include <kmenubar.h>
#include <kstandardshortcut.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <klocale.h>       // i18n()
#include <kstandardaction.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstatusbar.h>
#include <kactioncollection.h>
#include <ktoggleaction.h>
#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include <kxmlguifactory.h>
#include <kglobal.h>
#include "kticon.h"
#include "ktview.h"
#include <ktoolbar.h>
#include <kicon.h>
#include <QLabel>


const int KTApp::statusMessage            (1001);


KTApp::KTApp() : KXmlGuiWindow(0),
  config(KGlobal::config())
{
  setWindowIcon(KTIcon::application(true));

  setCaption(i18n("Task Scheduler"));

  // Call inits to invoke all other construction parts.
  setupActions();
  initStatusBar();

  // Read options.
  readOptions();

  // Initialize document.
  cthost = new CTHost(crontab);

  setupGUI(QSize(600, 320));

  // Initialize view.
  view = new KTView(this);
  setCentralWidget(view);

  //Connections
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this));
  connect(editMenu,SIGNAL(hovered(QAction*)),this,SLOT(statusEditCallback(QAction*)));
}

bool KTApp::init()
{
  if (cthost->isError())
  {
    KMessageBox::error(this, i18n("The following error occurred while initializing KCron:"
                                  "\n\n%1\n\nKCron will now exit.\n", cthost->errorMessage()));
    return false;
  }

  // Display greeting screen.
  // if there currently are no scheduled tasks...
  if (!cthost->root())
  {
    int taskCount(0);

    for (CTCronIterator i = (CTCronIterator)cthost->cron.begin();
      i != cthost->cron.end(); i++)
    {
      for (CTTaskIterator j = (CTTaskIterator)(*i)->task.begin();
        j != (*i)->task.end(); j++)
      {
        taskCount++;
      }
    }

    if (taskCount == 0)
    {
      show();
      KMessageBox::information(this, i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on the Tasks folder and select Edit/New from the menu."), i18n("Welcome to the Task Scheduler"), "welcome");
    }
  }
  return true;
}

KTApp::~KTApp()
{
 delete view;
 delete cthost;
}

const CTHost& KTApp::getCTHost() const
{
  return *cthost;
}


QString KTApp::caption()
{
  QString cap(KGlobal::caption());
  return cap;
}

void KTApp::setupActions()
{
  //File Menu
  KStandardAction::save(this, SLOT(slotFileSave()), actionCollection());
  KStandardAction::print(this, SLOT(slotFilePrint()), actionCollection());
  KStandardAction::quit(this, SLOT(slotFileQuit()), actionCollection());

  //Edit menu
  QAction *a = KStandardAction::cut(this, SLOT(slotEditCut()), actionCollection());
  actionCollection()->addAction( "edit_cut", a );
  KStandardAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  KStandardAction::paste(this, SLOT(slotEditPaste()), actionCollection());

  QAction* newAct = actionCollection()->addAction( "edit_new" );
  newAct->setObjectName("edit_new");
  newAct->setText( i18n("&New...") );
  newAct->setIcon( KIcon("document-new") );
  qobject_cast<KAction*>( newAct )->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::New));
  connect(newAct, SIGNAL(triggered(bool)), SLOT(slotEditNew()));

  //I don't like this KStandardShortcut::open() for modifying, but I'm just porting this to xmlui
  QAction *modifyAct = actionCollection()->addAction( "edit_modify" );
  modifyAct->setObjectName("edit_modify");
  modifyAct->setText( i18n("M&odify...") );
  modifyAct->setIcon( KIcon("document-open") );
  qobject_cast<KAction*>( modifyAct )->setShortcut(KStandardShortcut::shortcut(KStandardShortcut::Open));
  connect(modifyAct, SIGNAL(triggered(bool)), SLOT(slotEditModify()));

  QAction *deleteAct = actionCollection()->addAction( "edit_delete" );
  deleteAct->setObjectName("edit_delete");
  deleteAct->setText( i18n("&Delete") );
  deleteAct->setIcon( KIcon("edit-delete") );
  connect(deleteAct, SIGNAL(triggered(bool)), SLOT(slotEditDelete()));

  QAction *enableAct = actionCollection()->addAction( "edit_enable" );
  enableAct->setObjectName("edit_enable");
  enableAct->setText( i18n("&Enabled") );
  connect(enableAct, SIGNAL(triggered(bool)), SLOT(slotEditEnable()));

  QAction *runAct = actionCollection()->addAction( "edit_run" );
  runAct->setObjectName("edit_run");
  runAct->setText( i18n("&Run Now") );
  connect(runAct, SIGNAL(triggered(bool)), SLOT(slotEditRunNow()));
}

void KTApp::initStatusBar()
{
    QLabel *defaultmsg = new QLabel(i18n(" Ready."));
  defaultmsg->setFixedHeight( fontMetrics().height() + 2 );
  defaultmsg->setAlignment( Qt::AlignLeft | Qt::AlignVCenter );
  statusBar()->addWidget(defaultmsg);    
}

void KTApp::saveOptions()
{
  KConfigGroup group(config, "General Options");
  group.writeEntry(QString("Path to crontab"), crontab);
}


void KTApp::readOptions()
{
  KConfigGroup group(config, "General Options");

  // get the path to the crontab binary
  crontab = group.readEntry(QString("Path to crontab"), QString("crontab"));
}

bool KTApp::queryClose()
{
  if(cthost->dirty())
  {
    KTApp* win = (KTApp*)parent();

    int retVal = KMessageBox::warningYesNoCancel(win,
      i18n("Scheduled tasks have been modified.\nDo you want to save changes?"),
      QString::null,
      KStandardGuiItem::save(), KStandardGuiItem::discard()
      );

    switch (retVal)
    {
      case KMessageBox::Yes:
        cthost->apply();
        if (cthost->isError())
        {
           KMessageBox::error(win, cthost->errorMessage());
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
  }
  else
  {
    return true;
  }
}

bool KTApp::queryExit()
{
  saveOptions();
  return true;
}

void KTApp::slotFileSave()
{
  slotStatusMsg(i18n("Saving..."));
  cthost->apply();
  slotStatusMsg(i18n("Ready."));
  if (cthost->isError())
  {
     KMessageBox::error(this, cthost->errorMessage());
  }
}

void KTApp::slotFilePrint()
{
  slotStatusMsg(i18n("Printing..."));
  view->print();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotFileQuit()
{
  saveOptions();
  close();
}

void KTApp::slotEdit(const QPoint& qp)
{
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this));
  editMenu->exec(qp, 0);
}

void KTApp::slotEditCut()
{
  slotStatusMsg(i18n("Cutting to clipboard..."));
  view->copy();
  view->remove();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditCopy()
{
  slotStatusMsg(i18n("Copying to clipboard..."));
  view->copy();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditPaste()
{
  slotStatusMsg(i18n("Pasting from clipboard..."));
  view->paste();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditNew()
{
  slotStatusMsg(i18n("Adding new entry..."));
  view->create();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditModify()
{
  slotStatusMsg(i18n("Modifying entry..."));
  view->edit();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditDelete()
{
  slotStatusMsg(i18n("Deleting entry..."));
  view->remove();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditEnable()
{
  QAction* editEnable = actionCollection()->action("edit_enable");
  if (editEnable->isChecked())
  {
    slotStatusMsg(i18n("Disabling entry..."));
    view->enable(false);
    editEnable->setChecked(false);
  }
  else
  {
    slotStatusMsg(i18n("Enabling entry..."));
    view->enable(true);
    editEnable->setChecked(true);
  }
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditRunNow()
{
  slotStatusMsg(i18n("Running command..."));
  view->run();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotStatusMsg(const QString & text)
{
  statusBar()->showMessage(text);
  setCaption(i18n("Task Scheduler"), cthost->dirty());
}

void KTApp::slotStatusHelpMsg(const QString & text)
{
  statusBar()->showMessage(text, 2000);
}

void KTApp::statusEditCallback(QAction* action)
{
  QString text = action->objectName();
  if (text == "edit_new") {
    slotStatusHelpMsg(i18n("Create a new task or variable."));
  } else if (text == "edit_modify") {
    slotStatusHelpMsg(i18n("Edit the selected task or variable."));
  } else if (text == "edit_delete") {
    slotStatusHelpMsg(i18n("Delete the selected task or variable."));
  } else if (text == "edit_enable") {
    slotStatusHelpMsg(i18n("Enable/disable the selected task or variable."));
  } else if (text == "edit_run") {
    slotStatusHelpMsg(i18n("Run the selected task now."));
  }
}

void KTApp::slotEnableModificationButtons(bool state)
{
  if (state)
    stateChanged("no_task_selected", StateReverse);
  else
    stateChanged("no_task_selected");

}

void KTApp::slotEnablePaste(bool state)
{
 if (state)
    stateChanged("paste_disabled", StateReverse);
 else
    stateChanged("paste_disabled");
}

void KTApp::slotEnableRunNow(bool state)
{
  if (state)
    stateChanged("runnow_enabled");
  else
    stateChanged("runnow_enabled", StateReverse);
}

void KTApp::slotEnableEnabled(bool state)
{
  actionCollection()->action("edit_enable")->setChecked(state);
}

#include "ktapp.moc"
