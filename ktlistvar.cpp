/***************************************************************************
 *   ktlistvar.cpp                                                         *
 *   --------------------------------------------------------------------  *
 *   KT list view item variable implementation.                            *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ktlistvar.h"
#include "ktprint.h"

#include <klocale.h>  // i18n()

#include "ctvariable.h"

#include "kticon.h"
#include "ktvariable.h"

KTListVar::KTListVar(KTListItem* parent, CTCron* _ctcron,
  CTVariable* _ctvariable) :
  KTListItem(parent, 0, _ctcron),
  ctvar(_ctvariable)
{
  refresh();
  parent->setOpen(true);
}

void KTListVar::refresh()
{
  setText (0,QString::fromLocal8Bit(ctvar->variable.c_str()));

  if (ctvar->enabled)
  {
    setText (1,QString::fromLocal8Bit(ctvar->value.c_str()));
    setText (2,QString::fromLocal8Bit(ctvar->comment.c_str()));
  }
  else
  {
    setText(1, "");
    setText(2, i18n("Disabled"));
  }

  if (ctvar->variable == "MAILTO")
    setPixmap(0, KTIcon::mail(true));
  else if (ctvar->variable == "SHELL")
    setPixmap(0, KTIcon::shell(true));
  else if (ctvar->variable == "HOME")
    setPixmap(0, KTIcon::home(true));
  else if (ctvar->variable == "PATH")
    setPixmap(0, KTIcon::path(true));
  else
    setPixmap(0, KTIcon::variable(true));
}

void KTListVar::print(KTPrint& printer) const
{
  printer.print(QString::fromLocal8Bit(ctvar->variable.c_str()), 1, KTPrint::alignTextLeft);
  if (ctvar->enabled) {
    printer.print(QString::fromLocal8Bit(ctvar->value.c_str()),2, KTPrint::alignTextCenter);
    printer.print(QString::fromLocal8Bit(ctvar->comment.c_str()),3,KTPrint::alignTextRight);
  }
  else
    printer.print(i18n("Disabled."),3, KTPrint::alignTextRight);
		
}

void KTListVar::edit()
{
  KTVariable(ctvar).exec();
  refresh();
  parent()->sortChildItems(1, true);
}

CTVariable* KTListVar::getCTVariable() const
{
  return ctvar;
}
