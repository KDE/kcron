/***************************************************************************
 *   ktlisttask.cpp                                                        *
 *   --------------------------------------------------------------------  *
 *   KT list view item task implementation.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include <klocale.h>  // i18n()

#include "cttask.h"

#include "ktlisttask.h"
#include "kticon.h"
#include "kttask.h"
#include "ktprint.h"

KTListTask::KTListTask(KTListItem* parent,
  CTCron* _ctcron, CTTask* _cttask) :
  KTListItem(parent, 0, _ctcron),
  cttask(_cttask)

{
  refresh();
  parent->setOpen(true);
}

void KTListTask::refresh()
{
  setText(0, QString::fromLocal8Bit(cttask->comment.c_str()));

  if (cttask->enabled)
  {
    setText(1, QString::fromLocal8Bit(cttask->command.c_str()));
    setText(2, QString::fromLocal8Bit(cttask->describe().c_str()));
  }
  else
  {
    setText(1, "");
    setText(2, i18n("Disabled"));
  }

  setPixmap(0, KTIcon::task(true));
}

void KTListTask::print (KTPrint &printer) const
{    	
  printer.print(QString::fromLocal8Bit(cttask->comment.c_str()), 1, KTPrint::alignTextLeft);
  if (cttask->enabled) {
    printer.print(QString::fromLocal8Bit(cttask->command.c_str()),2, KTPrint::alignTextCenter);
    printer.print(QString::fromLocal8Bit(cttask->describe().c_str()),3, KTPrint::alignTextRight);
  }
  else
    printer.print(i18n("Disabled."), 3, KTPrint::alignTextRight);
}

void KTListTask::edit()
{
  KTTask(cttask).exec();
  refresh();
  parent()->sortChildItems(1, true);
}

CTTask* KTListTask::getCTTask() const
{
  return cttask;
}
