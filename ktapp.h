/***************************************************************************
 *   ktapp.h                                                               *
 *   --------------------------------------------------------------------  *
 *   KT application header.                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTAPP_H
#define KTAPP_H
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

class KAction;
class QString;
class KMenuBar;
class KAccel;
class KTView;
class CTHost;

/**
  * Application that sets up the main window, reads the config file,
  * sets up the menu bar, toolbar, and status bar.  Obtains the document
  * (here the crontab entries) and give it to the view for display.
  *
  * Provides main window handling, session management and keyboard
  * acceleration.
  */
class KTApp : public KMainWindow
{
  Q_OBJECT

  friend class KTView;

public:

  // Menu constants
  static const int menuEditNew;
  static const int menuEditModify;
  static const int menuEditDelete;
  static const int menuEditEnabled;
  static const int menuEditRunNow;
  static const int menuOptionsShowToolbar;
  static const int menuOptionsShowStatusbar;
  static const int menuHelpContents;
  static const int statusMessage;

/**
  * Initialize the application.
  */
  KTApp();

/**
  * Quit the application..
  */
  ~KTApp();

/**
 * Additional init
 */
 bool init();

/**
  * Enable menu item/toolbar item.  Id is one of the static constants 
  * identified above.
  */
  void enableCommand(int id_, bool enable = true);

/**
  * Enable enable.
  */
  void enableEnable(bool display = true, bool enable = true);

/**
  * Returns a reference to the document.
  */	
  const CTHost& getCTHost() const; 	

/**
  * Actions
  */
  KAction *actionSave, *actionPrint, *actionCut, *actionCopy, *actionPaste;

protected:

/**
  * Resize application window contents.
. */
  virtual void resizeEvent (QResizeEvent*);

/** Called on window close event.  Asks the document if it is dirty
  * and if so, prompts the user for saving before exiting..
  */
  virtual bool queryClose();

/** Called when the last window of the application is going to be
  * closed.  Saves options.
  */
  virtual bool queryExit();

public slots:

/**
  * Switch argument for slot selection by menu or toolbar ID.
  */
  void commandCallback(int id_);

/**
  * Switch argument for satus ar help entries on slot selection. Add your
  * ID's help here for toolbars and menubar entries.
  */
  void statusCallback(int id_);

/**
  * Save document.
  */
  void slotFileSave();

/**
  * Print document.
  */
  void slotFilePrint();

/**
  * Close all open windows then quits the application.  If queryClose()
  * returns false because the user canceled the saveModified() dialog, the
  * closing breaks.
  */
  void slotFileQuit();

/**
  * Pop up an edit menu.
  */
  void slotEdit(const QPoint& qp);

/**
  * Put the marked objects on the clipboard and remove it from the
  * document.
  */
  void slotEditCut();

/**
  * Put the marked objects on the clipboard.
  */
  void slotEditCopy();

/**
  * Paste the object on clipboard into the document
  */
  void slotEditPaste();

/**
  * New.
  */
  void slotEditNew();

/**
  * Modify.
  */
  void slotEditModify();

/**
  * Delete.
  */
  void slotEditDelete();

/**
  * Toggle enabled.
  */
  void slotEditEnable();

/**
  * Run program now.
  */
  void slotEditRunNow();

/**
  * Toggle the toolbar being visible.
  */
  void slotViewToolBar();

/**
  * Toggles the status bar being visible.
  */
  void slotViewStatusBar();

/**
  * Changes the status bar contents for the standard label permanently;
  * used to indicate current actions.
  */
  void slotStatusMsg(const QString & text);

/**
  * Changes the status message of the whole status bar for two seconds,
  * then restores the last status. This is used to display status bar
  * messages that give information about actions for toolbar icons and
  * menu entries.
  */
  void slotStatusHelpMsg(const QString & text);

private:

/**
  * Disabled copy constructor.
  */
  KTApp(const KTApp& source);

/**
  * Disabled assignment operator.
  */
  void operator = (const KTApp& source);

/**
  * Get application caption.
  */
  QString caption();

/**
  * Initialize menus.
  */
  void initMenuBar();

/**
  * Initialize actions.
  */
  void initActions();

/**
  * Initialize toolbar.
  */
  void initToolBar();

/**
  * Initialize status bar.
  */
  void initStatusBar();

/**
  * Initialize keyboard accelerators.
  */
  void initKeyAccel();

/**
  * Read general options again and initialize all variables like the
  * geometry.
  */
  void readOptions();

/**
  * Save general options like all bar positions and status as well as the
  * geometry to the configuration file.
  */ 	
  void saveOptions();

/**
  * File menu.
  */
  QPopupMenu* file_menu;

/**
  * Edit menu.
  */
  QPopupMenu* edit_menu;

/**
  * View menu.
  */
  QPopupMenu* view_menu;

/**
  * Help menu.
  */
  QPopupMenu* help_menu;

/**
  * Configuration object of the application.
  */
  KConfig* config;

/**
  * Key accelerator.
  */
  KAccel* key_accel;

/**
  * Main GUI view/working area.
  */
  KTView* view;

/**
  * Document object, here crotab enries.
  */
  CTHost* cthost;

/**
  * Menu bar
  */
  KMenuBar* menuBar;

};

#endif // KTAPP_H
