/***************************************************************************
 *   ktlisttask.h                                                          *
 *   --------------------------------------------------------------------  *
 *   KT list view item task header.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTTASK_H
#define KTLISTTASK_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include "ktlistitem.h"

class CTTask;

/**
  * QListViewItem with a CTTask.
  */
class KTListTask : public KTListItem
{
public:

/**
  * Initialize the list view item and task.
  */
  KTListTask(KTListItem* parent, CTCron* _ctcron, CTTask* _cttask);

/**
  * Refresh from underlying task.
  */
  virtual void refresh();

/**
  * Print task.
  */
  virtual void print(KTPrint &printer) const;

/**
  * Edit task.
  */
  virtual void edit();

/**
  * Get the task.
  */
  CTTask* getCTTask() const;

private:

/**
  * Task.
  */
  CTTask* cttask;
};

#endif // KTLISTTASK_H
