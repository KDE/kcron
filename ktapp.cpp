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
#include <kapp.h>
#include <klocale.h>       // i18n()

#include "cthost.h"
#include "ctcron.h"
#include "cttask.h"

#include "kticon.h"
#include "ktview.h"

const int KTApp::menuFileSave             (10060);
const int KTApp::menuFilePrint            (10070);
const int KTApp::menuFileQuit             (10090);
const int KTApp::menuEditCut              (11020);
const int KTApp::menuEditCopy             (11010);
const int KTApp::menuEditPaste            (11030);
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
  // setupMenu();
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
      KMessageBox::information(0L, i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on the Tasks folder and select Edit/New from the menu."), i18n("Welcome to the Task Scheduler"), "welcome");
    }
  }
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

void KTApp::setupMenu()
{

}
void KTApp::initMenuBar()
{
  file_menu = new QPopupMenu();
  file_menu->insertItem(KTIcon::save() ,i18n("&Save"), menuFileSave);
  file_menu->insertSeparator();
  file_menu->insertItem(KTIcon::print(), i18n("&Print"), menuFilePrint);
  file_menu->insertSeparator();
  file_menu->insertItem(i18n("&Quit"), menuFileQuit );

  edit_menu = new QPopupMenu();
  edit_menu->setCheckable(true);
  edit_menu->insertItem(KTIcon::cut(), i18n("Cu&t"), menuEditCut);
  edit_menu->insertItem(KTIcon::copy(), i18n("&Copy"), menuEditCopy);
  edit_menu->insertItem(KTIcon::paste(), i18n("&Paste"), menuEditPaste);
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
  menuBar->insertItem(i18n("&Options"), view_menu);
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
  toolBar()->insertButton(KTIcon::save(), menuFileSave, true, i18n("Save"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(KTIcon::print(), menuFilePrint, true, i18n("Print"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(KTIcon::cut(), menuEditCut, true, i18n("Cut"));
  toolBar()->insertButton(KTIcon::copy(), menuEditCopy, true, i18n("Copy"));
  toolBar()->insertButton(KTIcon::paste(), menuEditPaste, true, i18n("Paste"));
//  toolBar()->insertSeparator();
//  toolBar()->insertButton(KTIcon::help(), menuHelpContents, SIGNAL(clicked()),
//    kapp, SLOT(appHelpActivated()), true, i18n("Help"));

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

  // file menu accelerators
  key_accel->connectItem(KStdAccel::Save, this, SLOT(slotFileSave()));
  key_accel->connectItem(KStdAccel::Print, this, SLOT(slotFilePrint()));
  key_accel->connectItem(KStdAccel::Quit, this, SLOT(slotFileQuit()));

  // edit menu accelerators
  key_accel->connectItem(KStdAccel::Cut, this, SLOT(slotEditCut()));
  key_accel->connectItem(KStdAccel::Copy, this, SLOT(slotEditCopy()));
  key_accel->connectItem(KStdAccel::Paste, this, SLOT(slotEditPaste()));
  key_accel->connectItem(KStdAccel::New, this, SLOT(slotEditNew()));
  key_accel->connectItem(KStdAccel::Insert, this, SLOT(slotEditNew()));
  key_accel->connectItem(KStdAccel::Open, this, SLOT(slotEditModify()));

  // help menu accelerators
//  key_accel->connectItem(KStdAccel::Help, kapp, SLOT(appHelpActivated()));

  key_accel->changeMenuAccel(file_menu, menuFileSave, KStdAccel::Save);
  key_accel->changeMenuAccel(file_menu, menuFileQuit, KStdAccel::Quit);

  key_accel->changeMenuAccel(edit_menu, menuEditCut, KStdAccel::Cut);
  key_accel->changeMenuAccel(edit_menu, menuEditCopy, KStdAccel::Copy);
  key_accel->changeMenuAccel(edit_menu, menuEditPaste, KStdAccel::Paste);

  key_accel->changeMenuAccel(edit_menu, menuEditNew, KStdAccel::Insert);
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

    case menuFileSave:
    	slotFileSave();
    	break;
    case menuFilePrint:
    	slotFilePrint();
    	break;
    case menuFileQuit:
    	slotFileQuit();
    	break;

    case menuEditCut:
    	slotEditCut();
    	break;
    case menuEditCopy:
    	slotEditCopy();
    	break;
    case menuEditPaste:
    	slotEditPaste();
    	break;
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

    case menuFileSave:
      slotStatusHelpMsg(i18n("Save tasks and variables."));
      break;
    case menuFilePrint:
      slotStatusHelpMsg(i18n("Print all or current crontab(s)."));
      break;
    case menuFileQuit:
      slotStatusHelpMsg(i18n("Exit %1.").arg(caption()));
      break;

    case menuEditCut:
      slotStatusHelpMsg(i18n("Cut the selected task or variable and put on the clipboard."));
      break;
    case menuEditCopy:
      slotStatusHelpMsg(i18n("Copy the selected task or variable to the clipboard."));
      break;
    case menuEditPaste:
      slotStatusHelpMsg(i18n("Paste task or variable from the clipboard."));
      break;
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
