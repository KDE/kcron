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

#include "ktlistitem.h"

class QString;

/**
  * QListViewItem of a "variables" folder.
  */
class KTListVars : public KTListItem
{
public:

/**
  * Construct variables folder from branch.
  */
  KTListVars(KTListItem* parent, CTCron* _ctcron);

/**
  * Construct variables folder from root.
  */
  KTListVars(QListView* parent, CTCron* _ctcron);

/**
  * Internationalized description.
  */
  static QString getDescription();

/**
  * Refresh.
  */
  void refresh();

/**
  * print all variables.
  */
   void print(KTPrint &printer) const;

/**
  * Edit.
  */
  void create();

};

#endif // KTLISTVARS_H
