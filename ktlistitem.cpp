/***************************************************************************
 *   KT list view item abstract base class implementation.                 *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ktlistitem.h"
#include "ktprint.h"

#include "ctcron.h"

KTListItem::KTListItem(KTListItem* parent, const char* name, CTCron* _ctcron) :
  QListViewItem(parent, name),
  ctcron(_ctcron)
{
}

KTListItem::KTListItem(QListView* parent, const char* name, CTCron* _ctcron) :
  QListViewItem(parent, name),
  ctcron(_ctcron)
{
}

KTListItem::~KTListItem()
{
}

void KTListItem::create()
{
  return;
}

void KTListItem::edit()
{
  return;
}

CTCron* KTListItem::getCTCron() const
{
  return ctcron;
}
