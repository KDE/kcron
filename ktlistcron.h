/***************************************************************************
 *   ktlistcron.h                                                          *
 *   --------------------------------------------------------------------  *
 *   KT list view item cron header.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTCRON_H
#define KTLISTCRON_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qlistview.h>
#include <qstring.h>

#include "ctcron.h"
#include "kticon.h"

/**
  * An enhanced QListViewItem that keeps a pointer to a CTTask.
  */
class KTListCron : public QListViewItem
{
public:

/**
  * Construct tasks or variables folder from branch.
  */
  KTListCron(QListViewItem* parent, const char* name, CTCron* _ctcron) :
    QListViewItem(parent, name),
    ctcron(_ctcron)
  {
    refresh(name);
  };

/**
  * Construct tasks or variables folder from root.
  */
  KTListCron(QListView* parent, const char* name, CTCron* _ctcron) :
    QListViewItem(parent, name),
    ctcron(_ctcron)
  {
    refresh(name);
  };

/**
  * Construct user folder from root.
  */
  KTListCron(QListView* parent, CTCron* _ctcron) :
    QListViewItem(parent),
    ctcron(_ctcron)
  {
    refresh();
  };

/**
  * Refresh.
  */
  void refresh(const char* name = 0)
  {
    if (!name)
    {
      QString userName(ctcron->login.c_str());

      if (ctcron->login != ctcron->name)
      {
        userName += QString(" (");
        userName += ctcron->name.c_str();
        userName += QString(")");
      }
      setText(0, userName);

      if (ctcron->login == "system")
        setPixmap(0, KTIcon::system(true));
      else
        setPixmap(0, KTIcon::user(true));
    }
  };

/**
  * Get the user's crontab.
  */
  CTCron* getCTCron() const
  {
    return ctcron;
  }

private:

/**
  * User's crontab.
  */
  CTCron* ctcron;

};

#endif // KTLISTCRON_H




