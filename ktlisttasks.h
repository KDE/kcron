/***************************************************************************
 *   KT list view item cron tasks folder.                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTTASKS_H
#define KTLISTTASKS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include "ktlistitem.h"

class QString;

/**
  * QListViewItem of a "tasks" folder.
  */
class KTListTasks : public KTListItem
{
public:

/**
  * Construct tasks folder from branch.
  */
  KTListTasks(KTListItem* parent, CTCron* _ctcron);

/**
  * Construct tasks folder from root.
  */
  KTListTasks(QListView* parent, CTCron* _ctcron);

 /**
  * Internationalized description.
  */
  static QString getDescription();

/**
  * Create.
  */
  virtual void create();

/**
  * Refresh.
  */
  virtual void refresh();

/**
  * Print all tasks.
  */
  virtual void print(KTPrint &printer)const;

};

#endif // KTLISTTASKS_H
