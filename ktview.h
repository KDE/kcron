/***************************************************************************
 *   ktview.h                                                              *
 *   --------------------------------------------------------------------  *
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

#include <qwidget.h>

class QString;
class QListView;
class QListViewItem;
class KTApp;
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
  KTView (QWidget* parent = 0, const char* name=0);

/**
  * Destructor.
  */
  ~KTView();

/**
  * Print the view.
  */
  void print(QPrinter* m_pPrinter) const;

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

public slots:

/**
  * Pop up context sensitive menu.
  */
  void slotMenu(QListViewItem* qlvi, const QPoint& qp, int i);

/**
  * Default action, edit.
  */
  void slotEdit(QListViewItem* qlvi = 0);

protected slots:

/**
  * Set current and update menu
  */
  void slotSetCurrent (QListViewItem* qlvi);

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
  QListView* listView;

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






