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
#include <kstdaccel.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <klocale.h>       // i18n()
#include <kstdaction.h>
#include <kaction.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstatusbar.h>

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"
#include <kxmlguifactory.h>
#include <kglobal.h>
#include "kticon.h"
#include "ktview.h"


const int KTApp::statusMessage            (1001);


KTApp::KTApp() : KMainWindow(0)
{
  config=KGlobal::config();

  setIcon(KTIcon::application(true));

  setCaption(i18n("Task Scheduler"));


  // Initialize document.
  cthost = new CTHost();

  // Initialize view.
  view = new KTView(this);
  setCentralWidget(view);
  
  // Call inits to invoke all other construction parts.
  setupActions();
  initStatusBar();
  createGUI();
  
  //Connections
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this)); 
  KMenu *settingsMenu = static_cast<KMenu*>(guiFactory()->container("settings", this)); 
  
  connect(editMenu,SIGNAL(highlighted(int)),this,SLOT(statusEditCallback(int)));
  connect(settingsMenu,SIGNAL(highlighted(int)),this,SLOT(statusSettingsCallback(int)));
  // Read options.
  readOptions();
}

bool KTApp::init()
{
  if (cthost->isError())
  {
    KMessageBox::error(this, i18n("The following error occurred while initializing KCron:"
                                  "\n\n%1\n\nKCron will now exit.\n").arg(cthost->errorMessage()));
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
  QString cap(kapp->caption());
  return cap;
}

void KTApp::setupActions()
{
  //File Menu
  KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection());
  
  //Edit menu
  KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
  (void)new KAction(i18n("&New..."), KStdAccel::openNew(), this,SLOT(slotEditNew()),actionCollection(),"edit_new");
  //I don't like this KStdAccel::open() for modifying, but I'm just porting this to xmlui
  (void)new KAction(i18n("M&odify..."), KStdAccel::open(), this,SLOT(slotEditModify()),actionCollection(),"edit_modify");
  (void)new KAction(i18n("&Delete"), 0, this,SLOT(slotEditDelete()),actionCollection(),"edit_delete");
  (void)new KAction(i18n("&Enabled"), 0, this,SLOT(slotEditEnable()),actionCollection(),"edit_enable");
  (void)new KAction(i18n("&Run Now"), 0, this,SLOT(slotEditRunNow()),actionCollection(),"edit_run");
  
  //Settings menu
  (void)new KAction(i18n("Show &Toolbar"), 0, this,SLOT(slotViewToolBar()),actionCollection(),"show_toolbar");
  (void)new KAction(i18n("Show &Statusbar"), 0, this,SLOT(slotViewStatusBar()),actionCollection(),"show_statusbar");
    
}

void KTApp::initStatusBar()
{
  statusBar()->insertItem(i18n("Ready."), statusMessage, 1);
  statusBar()->setItemAlignment(statusMessage, Qt::AlignLeft | Qt::AlignVCenter);
}

void KTApp::saveOptions()
{
  config->setGroup(QString("General Options"));
  config->writeEntry(QString("Geometry"), size());
  config->writeEntry(QString("Show Toolbar"), toolBar()->isVisible());
  config->writeEntry(QString("Show Statusbar"), statusBar()->isVisible());
  config->writeEntry(QString("ToolBarPos"),  (int)toolBar()->barPos());
}


void KTApp::readOptions()
{
  config->setGroup(QString("General Options"));
  KMenu *settingsMenu = static_cast<KMenu*>(guiFactory()->container("settings", this));

  // bar status settings
  bool bViewToolbar = config->readBoolEntry(QString("Show Toolbar"), true);
  settingsMenu->setItemChecked(settingsMenu->idAt(0),bViewToolbar);
  if (!bViewToolbar)
    toolBar()->hide();

  bool bViewStatusbar = config->readBoolEntry(QString("Show Statusbar"), true);
  settingsMenu->setItemChecked(settingsMenu->idAt(1),bViewStatusbar);
  if (!bViewStatusbar)
    statusBar()->hide();

  // bar position settings
  KToolBar::BarPosition tool_bar_pos;
  tool_bar_pos=(KToolBar::BarPosition)
    config->readNumEntry(QString("ToolBarPos"),
    KToolBar::Top);

  toolBar()->setBarPos(tool_bar_pos);

  QSize size=config->readSizeEntry(QString("Geometry"));

  // Minimum size is 350 by 250

  if (size.isEmpty())
  {
    size.setWidth(350);
    size.setHeight(250);
  }

  if (size.width() < 350)
  {
    size.setWidth(350);
  }
  if (size.height() < 250)
  {
    size.setHeight(250);
  }

  resize(size);

}

bool KTApp::queryClose()
{
  if(cthost->dirty())
  {
    KTApp* win = (KTApp*)parent();

    int retVal = KMessageBox::warningYesNoCancel(win,
      i18n("Scheduled tasks have been modified.\nDo you want to save changes?"),
      QString::null,
      KStdGuiItem::save(), KStdGuiItem::discard()
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
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this)); 
  if (editMenu->isItemChecked(editMenu->idAt(8)))
  {
    slotStatusMsg(i18n("Disabling entry..."));
    view->enable(false);
    editMenu->setItemChecked(editMenu->idAt(8), false);
  }
  else
  {
    slotStatusMsg(i18n("Enabling entry..."));
    view->enable(true);
    editMenu->setItemChecked(editMenu->idAt(8), true);
  }
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotEditRunNow()
{
  slotStatusMsg(i18n("Running command..."));
  view->run();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotViewToolBar()
{
  if(toolBar()->isVisible())
    toolBar()->hide();
  else
    toolBar()->show();
  
  KMenu *settingsMenu = static_cast<KMenu*>(guiFactory()->container("settings", this));
  settingsMenu->setItemChecked(settingsMenu->idAt(0),toolBar()->isVisible());
  
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotViewStatusBar()
{
  if (statusBar()->isVisible())
    statusBar()->hide();
  else
    statusBar()->show();
  
  KMenu *settingsMenu = static_cast<KMenu*>(guiFactory()->container("settings", this));
  settingsMenu->setItemChecked(settingsMenu->idAt(1),statusBar()->isVisible());
  
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotStatusMsg(const QString & text)
{
  statusBar()->clear();
  statusBar()->changeItem(text, statusMessage);
  setCaption(i18n("Task Scheduler"), cthost->dirty());
}

void KTApp::slotStatusHelpMsg(const QString & text)
{
  statusBar()->message(text, 2000);
}

void KTApp::statusEditCallback(int id_)
{
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this)); 
  int index = editMenu->indexOf(id_);
  switch (index) {
    case menuEditNew:
      slotStatusHelpMsg(i18n("Create a new task or variable."));
      break;
    case menuEditModify:
      slotStatusHelpMsg(i18n("Edit the selected task or variable."));
      break;
    case menuEditDelete:
      slotStatusHelpMsg(i18n("Delete the selected task or variable."));
      break;
    case menuEditEnabled:
      slotStatusHelpMsg(i18n("Enable/disable the selected task or variable."));
      break;
    case menuEditRunNow:
      slotStatusHelpMsg(i18n("Run the selected task now."));
      break;

    default:
      break;
  }
}

void KTApp::statusSettingsCallback(int id_)
{
  KMenu *settingsMenu = static_cast<KMenu*>(guiFactory()->container("settings", this)); 
  int index = settingsMenu->indexOf(id_);
  switch (index) {
    case menuSettingsShowToolBar:
      slotStatusHelpMsg(i18n("Enable/disable the tool bar."));
      break;
    case menuSettingsShowStatusBar:
      slotStatusHelpMsg(i18n("Enable/disable the status bar."));
      break;
    
    default:
      break;
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
  KMenu *editMenu = static_cast<KMenu*>(guiFactory()->container("edit", this));
  editMenu->setItemChecked(editMenu->idAt(8),state);
}      

#include "ktapp.moc"

