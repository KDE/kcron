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
#include <kglobalsettings.h>

#include "ctcron.h"
#include "cttask.h"

#include "kticon.h"
#include "ktlisttask.h"
#include "kttask.h"
#include "ktprint.h"

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
  KTTask* kttask = new KTTask(temptask,i18n("Edit Task"));
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

void KTListTasks::print (KTPrint& printer) const
{
  QFont stnd;

  stnd = printer.getFont();

  printer.setFont(QFont( KGlobalSettings::generalFont().family(), 10, QFont::Bold ));
  printer.print (i18n("Task Name:"), 1, KTPrint::alignTextLeft);
  printer.print (i18n("Program:"), 2, KTPrint::alignTextCenter);
  printer.print (i18n("Description:"),3,KTPrint::alignTextRight);

  printer.setFont(stnd);

  //firstChild() does not return null if there are no children, therefore
  //we need to check the validation of the pointer without terminating
  //the application. This maybe a bug in QT 1.44

  if (this->childCount() ==0) {
    printer.print(i18n("No tasks..."),1,KTPrint::alignTextLeft, false);
    printer.levelColumns(20);
    return;
  }

  KTListItem* ktli = (KTListItem*)this->firstChild();
  Q_CHECK_PTR(ktli);

  while (ktli) {
    ktli->print(printer);
    ktli = (KTListItem*)ktli->nextSibling();
    printer.levelColumns();
  }
  printer.levelColumns(20);
}
