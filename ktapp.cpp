/***************************************************************************
 *   ktapp.cpp                                                             *
 *   --------------------------------------------------------------------  *
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
#include <kaccel.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kapplication.h>
#include <klocale.h>       // i18n()
#include <kstdaction.h>
#include <kaction.h>

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"

#include "kticon.h"
#include "ktview.h"
#include <kpopupmenu.h>
#include <kstatusbar.h>

const int KTApp::menuEditNew              (11040);
const int KTApp::menuEditModify           (11050);
const int KTApp::menuEditDelete           (11060);
const int KTApp::menuEditEnabled          (11070);
const int KTApp::menuEditRunNow           (11080);
const int KTApp::menuOptionsShowToolbar   (12010);
const int KTApp::menuOptionsShowStatusbar (12020);
const int KTApp::menuHelpContents         (1002);
const int KTApp::statusMessage            (1001);

KTApp::KTApp() : KMainWindow(0)
{
  config=kapp->config();

  setIcon(KTIcon::application(true));

  setCaption(i18n("Task Scheduler"));

  // Call inits to invoke all other construction parts.
  initActions();
  initMenuBar();
  initToolBar();
  initStatusBar();
  initKeyAccel();

  // Initialize document.
  cthost = new CTHost();

  // Initialize view.
  view = new KTView(this);
  setCentralWidget(view);

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

void KTApp::enableCommand(int id_, bool enable)
{
  // enable menu and toolbar functions by their ID's
  menuBar->setItemEnabled(id_, enable);
  toolBar()->setItemEnabled(id_, enable);
}

void KTApp::enableEnable(bool display, bool enable)
{
  edit_menu->setItemChecked(menuEditEnabled, display ? enable : false);
  enableCommand(menuEditEnabled, display);
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

void KTApp::initActions()
{
  actionSave = KStdAction::save(this, SLOT(slotFileSave()), actionCollection());
  actionPrint = KStdAction::print(this, SLOT(slotFilePrint()), actionCollection());
  actionCut = KStdAction::cut(this, SLOT(slotEditCut()), actionCollection());
  actionCopy = KStdAction::copy(this, SLOT(slotEditCopy()), actionCollection());
  actionPaste = KStdAction::paste(this, SLOT(slotEditPaste()), actionCollection());
}

void KTApp::initMenuBar()
{
  file_menu = new QPopupMenu();
  actionSave->plug(file_menu);
  file_menu->insertSeparator();
  actionPrint->plug(file_menu);
  file_menu->insertSeparator();
  KStdAction::quit(this, SLOT(slotFileQuit()), actionCollection())->plug(file_menu);

  edit_menu = new QPopupMenu();
  edit_menu->setCheckable(true);
  actionCut->plug(edit_menu);
  actionCopy->plug(edit_menu);
  actionPaste->plug(edit_menu);
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("&New..."), menuEditNew);
  edit_menu->insertItem(i18n("M&odify..."), menuEditModify);
  edit_menu->insertItem(i18n("&Delete"), menuEditDelete);
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("&Enabled"), menuEditEnabled);
  edit_menu->insertSeparator();
  edit_menu->insertItem(KTIcon::task(true), i18n("&Run Now"), menuEditRunNow);

  view_menu = new QPopupMenu();
  view_menu->setCheckable(true);
  view_menu->insertItem(i18n("Show &Toolbar"), menuOptionsShowToolbar);
  view_menu->insertItem(i18n("Show &Statusbar"), menuOptionsShowStatusbar);

  help_menu = new QPopupMenu();

  help_menu = helpMenu();

  menuBar = new KMenuBar(this);
  menuBar->insertItem(i18n("&File"), file_menu);
  menuBar->insertItem(i18n("&Edit"), edit_menu);
  menuBar->insertItem(i18n("&Settings"), view_menu);
  menuBar->insertSeparator();
  menuBar->insertItem(i18n("&Help"), help_menu);

  connect(file_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(file_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(edit_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(edit_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(view_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(view_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));
}

void KTApp::initToolBar()
{
  actionSave->plug(toolBar());
  toolBar()->insertSeparator();
  actionPrint->plug(toolBar());
  toolBar()->insertSeparator();
  actionCut->plug(toolBar());
  actionCopy->plug(toolBar());
  actionPaste->plug(toolBar());

  connect(toolBar(), SIGNAL(clicked(int)), SLOT(commandCallback(int)));
  connect(toolBar(), SIGNAL(pressed(int)), SLOT(statusCallback(int)));
}

void KTApp::initStatusBar()
{
  statusBar()->insertItem(i18n("Ready."), statusMessage, 1);
  statusBar()->setItemAlignment(statusMessage, Qt::AlignLeft | Qt::AlignVCenter);
}

void KTApp::initKeyAccel()
{
  key_accel = new KAccel(this);

  // edit menu accelerators
  key_accel->connectItem(KStdAccel::New, this, SLOT(slotEditNew()));
  //key_accel->connectItem(KStdAccel::Insert, this, SLOT(slotEditNew()));
  key_accel->connectItem(KStdAccel::Open, this, SLOT(slotEditModify()));

  // help menu accelerators
//  key_accel->connectItem(KStdAccel::Help, kapp, SLOT(appHelpActivated()));

  //key_accel->changeMenuAccel(edit_menu, menuEditNew, KStdAccel::Insert);
  key_accel->changeMenuAccel(edit_menu, menuEditNew, KStdAccel::New);
  key_accel->changeMenuAccel(edit_menu, menuEditModify, KStdAccel::Open);

  key_accel->readSettings();
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

  // bar status settings
  bool bViewToolbar = config->readBoolEntry(QString("Show Toolbar"), true);
  view_menu->setItemChecked(menuOptionsShowToolbar, bViewToolbar);
  if (!bViewToolbar)
    toolBar()->hide();

  bool bViewStatusbar = config->readBoolEntry(QString("Show Statusbar"), true);
  view_menu->setItemChecked(menuOptionsShowStatusbar, bViewStatusbar);
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
      i18n("Scheduled tasks have been modified.\nDo you want to save changes?")
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
  edit_menu->exec(qp, 0);
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
  if (edit_menu->isItemChecked(menuEditEnabled))
  {
    slotStatusMsg(i18n("Disabling entry..."));
    view->enable(false);
    edit_menu->setItemChecked(menuEditEnabled, false);
  }
  else
  {
    slotStatusMsg(i18n("Enabling entry..."));
    view->enable(true);
    edit_menu->setItemChecked(menuEditEnabled, true);
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
  bool visible;
  visible = !view_menu->isItemChecked(menuOptionsShowToolbar);
  view_menu->setItemChecked(menuOptionsShowToolbar, visible);
  if (visible)
     toolBar()->show();
  else
     toolBar()->hide();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotViewStatusBar()
{
  bool visible;
  visible = !view_menu->isItemChecked(menuOptionsShowStatusbar);
  view_menu->setItemChecked(menuOptionsShowStatusbar, visible);
  if (visible)
    statusBar()->show();
  else
    statusBar()->hide();
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

void KTApp::commandCallback(int id_){

  switch (id_){
    case menuEditNew:
    	slotEditNew();
    	break;
    case menuEditModify:
    	slotEditModify();
    	break;
    case menuEditDelete:
    	slotEditDelete();
    	break;
    case menuEditEnabled:
    	slotEditEnable();
    	break;
    case menuEditRunNow:
    	slotEditRunNow();
    	break;

    case menuOptionsShowToolbar:
    	slotViewToolBar();
    	break;
    case menuOptionsShowStatusbar:
    	slotViewStatusBar();
    	break;
    default:
    	break;
  }
}

void KTApp::statusCallback(int id_){

  switch (id_){

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

    case menuOptionsShowToolbar:
      slotStatusHelpMsg(i18n("Enable/disable the tool bar."));
      break;
    case menuOptionsShowStatusbar:
      slotStatusHelpMsg(i18n("Enable/disable the status bar."));
      break;

    default:
      break;
  }
}

void KTApp::resizeEvent (QResizeEvent* event)
{
  view->setGeometry(0, menuBar->height() +
    view_menu->isItemChecked(menuOptionsShowToolbar) * toolBar()->height(),
    width(), height() - menuBar->height() -
    view_menu->isItemChecked(menuOptionsShowToolbar) * toolBar()->height() -
    view_menu->isItemChecked(menuOptionsShowStatusbar) *
    statusBar()->height());

  repaint();
}

#include "ktapp.moc"
