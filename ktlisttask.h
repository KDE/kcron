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

#include <qlistview.h>
#include "ctcron.h"
#include "cttask.h"

/**
  * An enhanced QListViewItem that keeps a pointer to a CTTask.
  */
class KTListTask : public QListViewItem
{
public:

/**
  * Initialize the list view item and task.
  */
  KTListTask(QListViewItem* parent,
    CTCron* _ctcron,
    CTTask* _cttask) :
    QListViewItem(parent),
    ctcron(_ctcron),
    cttask(_cttask)
  {
    refresh();
    parent->setOpen(true);
  };

/**
  * Refresh from underlying task.
  */
  void refresh()
  {
    // Is this breaking 2-byte support?
    setText(0, cttask->comment.c_str());
    if (cttask->enabled)
    {
      setText(1, cttask->command.c_str());
      setText(2, cttask->describe().c_str());
    }
    else
    {
      setText(1, "");
      setText(2, i18n("Disabled"));
    }

    QString qs(cttask->command.c_str());
    QString iconName(qs.right(qs.length() - qs.findRev("/",
      qs.length()) - 1));
    iconName += ".xpm";
    QPixmap qp(KTIcon::getMiniIcon(iconName));
    if (qp.isNull())
      setPixmap(0, KTIcon::task(true));
    else
      setPixmap(0, qp);
  }

/**
  * Get the user's crontab.
  */
  CTCron* getCTCron() const
  {
    return ctcron;
  }

/**
  * Get the task.
  */
  CTTask* getCTTask() const
  {
    return cttask;
  }

private:

/**
  * User's crontab.
  */
  CTCron* ctcron;

/**
  * Task.
  */
  CTTask* cttask;
};

#endif // KTLISTTASK_H





