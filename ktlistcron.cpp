/***************************************************************************
 *   ktlistcron.cpp                                                        *
 *   --------------------------------------------------------------------  *
 *   KT list view item cron implementation.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ktlistcron.h"

#include <qstring.h>

#include "ctcron.h"

#include "kticon.h"

KTListCron::KTListCron(KTListItem* parent, const char* name,
  CTCron* _ctcron) :
  KTListItem(parent, name, _ctcron)
{
  refresh();
}

KTListCron::KTListCron(QListView* parent, const char* name,
  CTCron* _ctcron) :
  KTListItem(parent, name, _ctcron)
{
  refresh();
}

KTListCron::KTListCron(QListView* parent, CTCron* _ctcron) :
  KTListItem(parent, (const char*)0, _ctcron)
{
  refresh();
}

void KTListCron::refresh()
{
  setPixmap(0, getCTCron()->login.c_str() == "system" ?
    KTIcon::system(true) : KTIcon::user(true));

  QString userName(getCTCron()->login.c_str());

  if (getCTCron()->login != getCTCron()->name)
  {
    userName += QString(" (");
    userName += getCTCron()->name.c_str();
    userName += QString(")");
  }
  setText(0, userName);
}
