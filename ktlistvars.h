/***************************************************************************
 *   ktlistvars.h                                                          *
 *   --------------------------------------------------------------------  *
 *   KT list view item cron variables folder.                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTVARS_H
#define KTLISTVARS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qlistview.h>
#include <qstring.h>

#include "ctcron.h"
#include "kticon.h"

/**
  * An enhanced QListViewItem that is an environment variable.
  */
class KTListVars : public QListViewItem
{
public:

/**
  * Internationalized description.
  */
  static QString getDescription()
  {
    return i18n("Variables");
  };

/**
  * Construct variables folder from branch.
  */
  KTListVars(QListViewItem* parent, CTCron* _ctcron) :
    QListViewItem(parent),
    ctcron(_ctcron)
  {
    refresh();
  };

/**
  * Construct variables folder from root.
  */
  KTListVars(QListView* parent, CTCron* _ctcron) :
    QListViewItem(parent),
    ctcron(_ctcron)
  {
    refresh();
  };

/**
  * Refresh.
  */
  void refresh()
  {
    setText(0, getDescription());
    setPixmap(0, KTIcon::variables(true));
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

#endif // KTLISTVARS_H



