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

#include <kmenubar.h>
#include <kaccel.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <kconfig.h>
#include <kapp.h>
#include <klocale.h>

#include "ktapp.h"
#include "resource.h"
#include "kticon.h"
#include "ktview.h"

#include "cthost.h"

KTApp::KTApp()
{
#ifdef KDE1
  config=KApplication::getKApplication()->getConfig();
#else
  config=kapp->config();
#endif

  setIcon(KTIcon::application(true));

  setCaption(i18n("KDE Task Scheduler"));

  // Call inits to invoke all other construction parts.
  initMenuBar();
  initToolBar();
  initStatusBar();
  initKeyAccel();

  // Initialize document.
  cthost = new CTHost();

  // Initialize view.
  view = new KTView(this);
  setView(view);	

  // Read options.
  readOptions();
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

void KTApp::enableCut(bool enable)
{
  enableCommand(ID_EDIT_CUT, enable);
}

void KTApp::enableCopy(bool enable)
{
  enableCommand(ID_EDIT_COPY, enable);
}

void KTApp::enablePaste(bool enable)
{
  enableCommand(ID_EDIT_PASTE, enable);
}

void KTApp::enableNew(bool enable)
{
  enableCommand(ID_EDIT_NEW, enable);
}

void KTApp::enableModify(bool enable)
{
  enableCommand(ID_EDIT_MODIFY, enable);
}

void KTApp::enableDelete(bool enable)
{
  enableCommand(ID_EDIT_DELETE, enable);
}

void KTApp::enableRunNow(bool enable)
{
  enableCommand(ID_EDIT_RUNNOW, enable);
}

void KTApp::enableEnable(bool display, bool enable)
{
  edit_menu->setItemChecked(ID_EDIT_ENABLE,
    display ? enable : false);
  enableCommand(ID_EDIT_ENABLE, display);
}

const CTHost& KTApp::getCTHost() const
{
	return *cthost;
}

void KTApp::initMenuBar()
{
  file_menu = new QPopupMenu();
  file_menu->insertItem(KTIcon::save() ,i18n("&Save"), ID_FILE_SAVE );
  file_menu->insertSeparator();
  // file_menu->insertItem(KTIcon::print(), i18n("&Print..."), ID_FILE_PRINT );
  // file_menu->insertSeparator();
  file_menu->insertItem(i18n("&Quit"), ID_FILE_QUIT );

  edit_menu = new QPopupMenu();
  edit_menu->setCheckable(true);
  edit_menu->insertItem(KTIcon::cut(), i18n("Cu&t"), ID_EDIT_CUT );
  edit_menu->insertItem(KTIcon::copy(), i18n("&Copy"), ID_EDIT_COPY );
  edit_menu->insertItem(KTIcon::paste(), i18n("&Paste"), ID_EDIT_PASTE );
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("&New..."), ID_EDIT_NEW);
  edit_menu->insertItem(i18n("M&odify..."), ID_EDIT_MODIFY);
  edit_menu->insertItem(i18n("&Delete"), ID_EDIT_DELETE);
  edit_menu->insertSeparator();
  edit_menu->insertItem(i18n("&Enabled"), ID_EDIT_ENABLE);
  edit_menu->insertSeparator();
  edit_menu->insertItem(KTIcon::task(true), i18n("&Run Now"), ID_EDIT_RUNNOW);

  view_menu = new QPopupMenu();
  view_menu->setCheckable(true);
  view_menu->insertItem(i18n("Show &Tool Bar"), ID_VIEW_TOOLBAR);
  view_menu->insertItem(i18n("Show &Status Bar"), ID_VIEW_STATUSBAR);

  help_menu = new QPopupMenu();
#ifdef KDE1
  QString aboutstring=QString(
    kapp->getCaption())+
    " "+VERSION+"\n\n"+
    "(c) 1999 Gary Meyer <gary@meyer.net>";
  help_menu = KApplication::getKApplication()->getHelpMenu(true, aboutstring);
#else
  QString aboutstring=QString(
    kapp->caption())+
    " "+VERSION+"\n\n"+
    "(c) 1999 Gary Meyer <gary@meyer.net>";
  help_menu = helpMenu(aboutstring);
#endif

  menuBar = new KMenuBar(this);

  menuBar->insertItem(i18n("&File"), file_menu);
  menuBar->insertItem(i18n("&Edit"), edit_menu);
  menuBar->insertItem(i18n("&Options"), view_menu);

  menuBar->insertSeparator();
  menuBar->insertItem(i18n("&Help"), help_menu);

  setMenu(menuBar);

  connect(file_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(file_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(edit_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(edit_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));

  connect(view_menu,SIGNAL(activated(int)),SLOT(commandCallback(int)));
  connect(view_menu,SIGNAL(highlighted(int)),SLOT(statusCallback(int)));
}

void KTApp::initToolBar()
{
  toolBar()->insertButton(KTIcon::save(), ID_FILE_SAVE, true,
    i18n("Save"));
  // toolBar()->insertButton(KTIcon::print(), ID_FILE_PRINT, true,
  //   i18n("Print"));
  toolBar()->insertSeparator();
  toolBar()->insertButton(KTIcon::cut(), ID_EDIT_CUT, true,
    i18n("Cut"));
  toolBar()->insertButton(KTIcon::copy(), ID_EDIT_COPY, true,
    i18n("Copy"));
  toolBar()->insertButton(KTIcon::paste(), ID_EDIT_PASTE, true,
    i18n("Paste"));

  toolBar()->insertSeparator();

  toolBar()->insertButton(KTIcon::help(),
    ID_HELP_CONTENTS, SIGNAL(clicked()),
    kapp, SLOT(appHelpActivated()),
    true, i18n("Help"));

  connect(toolBar(), SIGNAL(clicked(int)), SLOT(commandCallback(int)));
  connect(toolBar(), SIGNAL(pressed(int)), SLOT(statusCallback(int)));
}

void KTApp::initStatusBar()
{
  statusBar()->insertItem(i18n("Ready."), ID_STATUS_MSG);
}

void KTApp::initKeyAccel()
{
  key_accel = new KAccel(this);

  // file menu accelerators
  key_accel->connectItem(KAccel::Save, this, SLOT(slotFileSave()));
  key_accel->connectItem(KAccel::Print, this, SLOT(slotFilePrint()));
  key_accel->connectItem(KAccel::Quit, this, SLOT(slotFileQuit()));

  // edit menu accelerators
  key_accel->connectItem(KAccel::Cut, this, SLOT(slotEditCut()));
  key_accel->connectItem(KAccel::Copy, this, SLOT(slotEditCopy()));
  key_accel->connectItem(KAccel::Paste, this, SLOT(slotEditPaste()));
  key_accel->connectItem(KAccel::New, this, SLOT(slotEditNew()));
  key_accel->connectItem(KAccel::Insert, this, SLOT(slotEditNew()));
  key_accel->connectItem(KAccel::Open, this, SLOT(slotEditModify()));

  // help menu accelerators
  key_accel->connectItem(KAccel::Help, kapp,
    SLOT(appHelpActivated()));

  key_accel->changeMenuAccel(file_menu, ID_FILE_SAVE, KAccel::Save);
  key_accel->changeMenuAccel(file_menu, ID_FILE_PRINT, KAccel::Print);
  key_accel->changeMenuAccel(file_menu, ID_FILE_QUIT, KAccel::Quit);

  key_accel->changeMenuAccel(edit_menu, ID_EDIT_CUT, KAccel::Cut);
  key_accel->changeMenuAccel(edit_menu, ID_EDIT_COPY, KAccel::Copy);
  key_accel->changeMenuAccel(edit_menu, ID_EDIT_PASTE, KAccel::Paste);

  key_accel->changeMenuAccel(edit_menu, ID_EDIT_NEW, KAccel::Insert);
  key_accel->changeMenuAccel(edit_menu, ID_EDIT_NEW, KAccel::New);
  key_accel->changeMenuAccel(edit_menu, ID_EDIT_MODIFY, KAccel::Open);

  key_accel->readSettings();
}

void KTApp::saveOptions()
{	
#ifdef KDE1
  config->setGroup("General Options");
  config->writeEntry("Geometry", size() );
  config->writeEntry("Show Toolbar", toolBar()->isVisible());
  config->writeEntry("Show Statusbar",statusBar()->isVisible());
  config->writeEntry("MenuBarPos", (int)menuBar->menuBarPos());
  config->writeEntry("ToolBarPos",  (int)toolBar()->barPos());
#else
  config->setGroup(QString("General Options"));
  config->writeEntry(QString("Geometry"), size());
  config->writeEntry(QString("Show Toolbar"), toolBar()->isVisible());
  config->writeEntry(QString("Show Statusbar"), statusBar()->isVisible());
  config->writeEntry(QString("MenuBarPos"), (int)menuBar->menuBarPos());
  config->writeEntry(QString("ToolBarPos"),  (int)toolBar()->barPos());
#endif
}


void KTApp::readOptions()
{
#ifdef KDE1
  config->setGroup("General Options");

  // bar status settings
  bool bViewToolbar = config->readBoolEntry("Show Toolbar", true);
  view_menu->setItemChecked(ID_VIEW_TOOLBAR, bViewToolbar);
  if (!bViewToolbar)
    enableToolBar(KToolBar::Hide);
	
  bool bViewStatusbar = config->readBoolEntry("Show Statusbar", true);
  view_menu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
  if (!bViewStatusbar)
    enableStatusBar(KStatusBar::Hide);
	
  // bar position settings
  KMenuBar::menuPosition menu_bar_pos;
  menu_bar_pos=(KMenuBar::menuPosition)config->readNumEntry("MenuBarPos",
    KMenuBar::Top);

  KToolBar::BarPosition tool_bar_pos;
  tool_bar_pos=(KToolBar::BarPosition)config->readNumEntry("ToolBarPos",
    KToolBar::Top);

  menuBar->setMenuBarPos(menu_bar_pos);
  toolBar()->setBarPos(tool_bar_pos);
	
  QSize size=config->readSizeEntry("Geometry");

  if (!size.isEmpty())
    resize(size);
#else
  config->setGroup(QString("General Options"));

  // bar status settings
  bool bViewToolbar = config->readBoolEntry(QString("Show Toolbar"), true);
  view_menu->setItemChecked(ID_VIEW_TOOLBAR, bViewToolbar);
  if (!bViewToolbar)
    enableToolBar(KToolBar::Hide);
	
  bool bViewStatusbar = config->readBoolEntry(QString("Show Statusbar"), true);
  view_menu->setItemChecked(ID_VIEW_STATUSBAR, bViewStatusbar);
  if (!bViewStatusbar)
    enableStatusBar(KStatusBar::Hide);
	
  // bar position settings
  KMenuBar::menuPosition menu_bar_pos;
  menu_bar_pos=(KMenuBar::menuPosition)
    config->readNumEntry(QString("MenuBarPos"),
    KMenuBar::Top);

  KToolBar::BarPosition tool_bar_pos;
  tool_bar_pos=(KToolBar::BarPosition)
    config->readNumEntry(QString("ToolBarPos"),
    KToolBar::Top);

  menuBar->setMenuBarPos(menu_bar_pos);
  toolBar()->setBarPos(tool_bar_pos);
	
  QSize size=config->readSizeEntry(QString("Geometry"));

  if (!size.isEmpty())
    resize(size);
#endif
}

bool KTApp::queryClose()
{
  if(cthost->dirty())
  {
    KTApp* win = (KTApp*)parent();

#ifdef KDE1
    int retVal = QMessageBox::information(win,
      kapp->getCaption(),
      i18n("Scheduled tasks have been modified.\nDo you want to save changes?"),
      QMessageBox::Yes | QMessageBox::Default,
      QMessageBox::No,
      QMessageBox::Cancel | QMessageBox::Escape);
#else
    int retVal = QMessageBox::information(win,
      kapp->caption(),
      i18n("Scheduled tasks have been modified.\nDo you want to save changes?"),
      QMessageBox::Yes | QMessageBox::Default,
      QMessageBox::No,
      QMessageBox::Cancel | QMessageBox::Escape);
#endif

    switch (retVal)
    {
      case QMessageBox::Yes:
        cthost->apply();
        return true;
        break;
      case QMessageBox::No:
        return true;
        break;	
      case QMessageBox::Cancel:
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
  return true;;
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
  QPrinter printer;
  if (printer.setup(this))
  {
    view->print(&printer);
  }
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
  if (edit_menu->isItemChecked(ID_EDIT_ENABLE))
  {
    slotStatusMsg(i18n("Disabling entry..."));
    view->enable(false);
    edit_menu->setItemChecked(ID_EDIT_ENABLE, false);
  }
  else
  {
    slotStatusMsg(i18n("Enabling entry..."));
    view->enable(true);
    edit_menu->setItemChecked(ID_EDIT_ENABLE, true);
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
  if (view_menu->isItemChecked(ID_VIEW_TOOLBAR))
    view_menu->setItemChecked(ID_VIEW_TOOLBAR, false);
  else
    view_menu->setItemChecked(ID_VIEW_TOOLBAR, true);
  enableToolBar();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotViewStatusBar()
{
  if (view_menu->isItemChecked(ID_VIEW_STATUSBAR))
    view_menu->setItemChecked(ID_VIEW_STATUSBAR, false);
  else
    view_menu->setItemChecked(ID_VIEW_STATUSBAR, true);
  enableStatusBar();
  slotStatusMsg(i18n("Ready."));
}

void KTApp::slotStatusMsg(const char *text)
{
  statusBar()->clear();
  statusBar()->changeItem(text, ID_STATUS_MSG);
}

void KTApp::slotStatusHelpMsg(const char *text)
{
  statusBar()->message(text, 2000);
}

void KTApp::commandCallback(int id_){

  switch (id_){

    case ID_FILE_SAVE:
    	slotFileSave();
    	break;
    case ID_FILE_PRINT:
    	slotFilePrint();
    	break;
    case ID_FILE_QUIT:
    	slotFileQuit();
    	break;

    case ID_EDIT_CUT:
    	slotEditCut();
    	break;
    case ID_EDIT_COPY:
    	slotEditCopy();
    	break;
    case ID_EDIT_PASTE:
    	slotEditPaste();
    	break;
    case ID_EDIT_NEW:
    	slotEditNew();
    	break;
    case ID_EDIT_MODIFY:
    	slotEditModify();
    	break;
    case ID_EDIT_DELETE:
    	slotEditDelete();
    	break;
    case ID_EDIT_ENABLE:
    	slotEditEnable();
    	break;
    case ID_EDIT_RUNNOW:
    	slotEditRunNow();
    	break;

    case ID_VIEW_TOOLBAR:
    	slotViewToolBar();
    	break;
    case ID_VIEW_STATUSBAR:
    	slotViewStatusBar();
    	break;
    default:
    	break;
  }
}

void KTApp::statusCallback(int id_){

  switch (id_){

    case ID_FILE_SAVE:
      slotStatusHelpMsg(i18n("Save tasks and variables."));
      break;
    case ID_FILE_PRINT:
      slotStatusHelpMsg(i18n("Print tasks and variables."));
      break;
    case ID_FILE_QUIT:
#ifdef KDE1
      slotStatusHelpMsg(i18n("Exit " +
        QString(kapp->getCaption()) + "."));
#else
      slotStatusHelpMsg(i18n("Exit " +
        QString(kapp->caption()) + "."));
#endif
      break;

    case ID_EDIT_CUT:
      slotStatusHelpMsg(i18n("Cut the selected task or variable and put on the clipboard."));
      break;
    case ID_EDIT_COPY:
      slotStatusHelpMsg(i18n("Copy the selected task or variable to the clipboard."));
      break;
    case ID_EDIT_PASTE:
      slotStatusHelpMsg(i18n("Paste task or variable from the clipboard."));
      break;
    case ID_EDIT_NEW:
      slotStatusHelpMsg(i18n("Create a new task or variable."));
      break;
    case ID_EDIT_MODIFY:
      slotStatusHelpMsg(i18n("Edit the selected task or variable."));
      break;
    case ID_EDIT_DELETE:
      slotStatusHelpMsg(i18n("Delete the selected task or variable."));
      break;
    case ID_EDIT_ENABLE:
      slotStatusHelpMsg(i18n("Enable/disable the selected task or variable."));
      break;
    case ID_EDIT_RUNNOW:
      slotStatusHelpMsg(i18n("Run the selected task now."));
      break;

    case ID_VIEW_TOOLBAR:
      slotStatusHelpMsg(i18n("Enables/disable the tool bar."));
      break;
    case ID_VIEW_STATUSBAR:
      slotStatusHelpMsg(i18n("Enable/disable the status bar."));
      break;

    default:
      break;
  }
}

void KTApp::resizeEvent (QResizeEvent* event)
{
  view->setGeometry(0, menuBar->height() +
    view_menu->isItemChecked(ID_VIEW_TOOLBAR) * toolBar()->height(),
    width(), height() - menuBar->height() -
    view_menu->isItemChecked(ID_VIEW_TOOLBAR) * toolBar()->height() -
    view_menu->isItemChecked(ID_VIEW_STATUSBAR) * statusBar()->height());

  repaint();
}





