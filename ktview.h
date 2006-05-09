/***************************************************************************
 *   KT main view header.                                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTVIEW_H
#define KTVIEW_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <QWidget>
//Added by qt3to4:
#include <QResizeEvent>

class QString;
class Q3ListView;
class Q3ListViewItem;
class KTApp;
class KTPrint;
class KTListItem;
class CTCron;
class CTVariable;
class CTTask;

/**
  * Main GUI view of the crontab entries.
  */
class KTView : public QWidget
{
  Q_OBJECT

public:

/**
  * Initializes view.
  */
  KTView (QWidget* parent = 0);

/**
  * Destructor.
  */
  ~KTView();

/**
  * Print crontab.
  */
  void print() const;

/**
  * Print page heading.
  */
  void pageHeading (KTListItem *user, KTPrint &printer) const;

/**
  * Print page footer.
  */
  void pageFooter (KTListItem *user, KTPrint &printer) const;

/**
  * Copies variables and/or tasks.
  */
  void copy();

/**
  * Pastes variables and/or tasks from the clipboard.
  */
  void paste();

/**
  * Create a new variable or task.  Default is which type is most recently
  * selected.
  */
  void create();

/**
  * Edit variable or task.  Default is most recently selected.
  */
  void edit();

/**
  * Delete variables and/or tasks.
  */
  void remove();

/**
  * Enables/disables variables and/or tasks.
  */
  void enable(bool enable = true);

/**
  * Run task now.
  */
  void run() const;

signals:

/** Enables/disables modification 
  * buttons depending
  * on if a task is selected
  */
  void enableModificationButtons(bool);

/** Enables/disables paste button depending
  * on clipboard contents
  */
  void enablePaste(bool);

/** Enables/disables "Run now" depending
  * on the task settings
  */
  void enableRunNow(bool);

/** Enables/disables "Activated" setting
  * on the menu depending on the task
  * configuration
  */
  void enableEnabled(bool);

public slots:

/**
  * Pop up context sensitive menu.
  */
  void slotMenu(Q3ListViewItem* qlvi, const QPoint& qp, int i);

/**
  * Default action, edit.
  */
  void slotEdit(Q3ListViewItem* qlvi = 0);

protected slots:

/**
  * Set current and update menu
  */
  void slotSetCurrent (Q3ListViewItem* qlvi);

protected:

/**
  * Resize view contents.
  */
  virtual void resizeEvent (QResizeEvent* = 0);

private:

/**
  * Get absolute path of command.
  */
  QString absolute() const;

/**
  * Refresh view from underlying objects.
  */
  void refresh();

/**
  * The application.
  */
  KTApp* ktapp;

/**
  * Tree view of the crontab entries.
  */
  Q3ListView* listView;

/**
  * Current user's crontab.
  */
  CTCron* currentCTCron;

/**
  * Indicates whether or not currently selected item is a task.
  */
  bool currentIsTask;

/**
  * Current task.
  */
  CTTask* currentCTTask;

/**
  * Current variable.
  */
  CTVariable* currentCTVariable;

/**
  * Indicates whether or not the item on the clipboard is a task.
  */
  bool clipboardIsTask;

/**
  * Clipboard task.
  */
  CTTask* clipboardCTTask;

/**
  * Clipboard variable.
  */
  CTVariable* clipboardCTVariable;

};

#endif // KTVIEW_H
