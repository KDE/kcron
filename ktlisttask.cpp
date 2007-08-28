/***************************************************************************
 *   KT list view item task implementation.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ktlisttask.h"

#include <klocale.h>  // i18n()

#include "cttask.h"

#include "kticon.h"
#include "kttask.h"
#include "ktprint.h"
#include "kiconloader.h"

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
    if (cttask->reboot) 
    {
      setText(1, QString::fromLocal8Bit(cttask->command.c_str()));
      setText(2, i18n("At system startup"));
    }
    else 
    {
      setText(1, QString::fromLocal8Bit(cttask->command.c_str()));
      setText(2, QString::fromLocal8Bit(cttask->describe().c_str()));
    }
  }
  else
  {
    setText(1, "");
    setText(2, i18nc("The cron task had been disabled", "Disabled"));
  }

  QString qsCommand(QString::fromLocal8Bit(cttask->command.c_str()));

  // qsCommand broken down this way to split off qsCommand attributes
  int firstSpace(qsCommand.indexOf(" "));
  if (firstSpace > 0) qsCommand = qsCommand.left(firstSpace);
  int lastSlash(qsCommand.lastIndexOf("/"));
  if (lastSlash > 0) qsCommand = qsCommand.right(qsCommand.size() - lastSlash - 1);

  // using KIconLoader() instead of getMaxIcon() because we need a null pixmap if pixmap cannot be found
  KIconLoader *loader = KIconLoader::global();
  QPixmap qpIcon(loader->loadIcon(qsCommand, K3Icon::Small, 0, K3Icon::DefaultState, QStringList(), 0L, true));
  if (qpIcon.isNull()) qpIcon = KTIcon::getMiniIcon("gear");
  setPixmap(0, qpIcon);
}

void KTListTask::print (KTPrint &printer) const
{
  printer.print(QString::fromLocal8Bit(cttask->comment.c_str()), 1, KTPrint::alignTextLeft);
  if (cttask->enabled) 
  {      
    printer.print(QString::fromLocal8Bit(cttask->command.c_str()),2, KTPrint::alignTextCenter);

    if (cttask->reboot) 
      printer.print(i18n("At system startup"), 3, KTPrint::alignTextRight);

    else 
      printer.print(QString::fromLocal8Bit(cttask->describe().c_str()),3, KTPrint::alignTextRight);
  }
  else
    printer.print(i18nc("The cron task had been disabled", "Disabled."), 3, KTPrint::alignTextRight);
}

void KTListTask::edit()
{
  KTTask(cttask,i18n("Modify Task")).exec();
  refresh();
  parent()->sortChildItems(1, true);
}

CTTask* KTListTask::getCTTask() const
{
  return cttask;
}
