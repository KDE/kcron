/***************************************************************************
 *   ktlisttasks.cpp                                                       *
 *   --------------------------------------------------------------------  *
 *   KT list view item tasks implementation.                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ktlisttasks.h"

#include <qstring.h>

#include <klocale.h>  // i18n()

#include "ctcron.h"
#include "cttask.h"

#include "kticon.h"
#include "ktlisttask.h"
#include "kttask.h"

KTListTasks::KTListTasks(KTListItem* parent, CTCron* _ctcron) :
  KTListItem(parent, 0, _ctcron)
{
  refresh();
}

KTListTasks::KTListTasks(QListView* parent, CTCron* _ctcron) :
  KTListItem(parent, 0, _ctcron)
{
  refresh();
}

QString KTListTasks::getDescription()
{
  return i18n("Tasks");
}

void KTListTasks::create()
{
  CTTask* temptask = new CTTask ("", "", getCTCron()->syscron);
  KTTask* kttask = new KTTask(temptask);
  kttask->exec();
  delete kttask;
  if (temptask->dirty())
  {
    getCTCron()->task.push_back(temptask);
    new KTListTask(this, getCTCron(), temptask); // Qt will clean up
  }
  else
  {
    delete temptask;
  }
}

void KTListTasks::refresh()
{
  setText(0, getDescription());
  setPixmap(0, KTIcon::tasks(true));
}
