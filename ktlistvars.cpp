/***************************************************************************
 *   ktlistvars.cpp                                                        *
 *   --------------------------------------------------------------------  *
 *   KT list view item cron variables folder implementation.               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ktlistvars.h"

#include "kticon.h"
#include "klocale.h"  // i18n()

#ifdef KDE1
#include "kapp.h"     // i18n()
#endif

#include <qstring.h>

#include "ktlistvar.h"
#include "ktvariable.h"
#include "ctvariable.h"
#include "ctcron.h"

KTListVars::KTListVars(KTListItem* parent, CTCron* _ctcron) :
  KTListItem(parent, 0, _ctcron)
{
  refresh();
}

KTListVars::KTListVars(QListView* parent, CTCron* _ctcron) :
  KTListItem(parent, 0, _ctcron)
{
  refresh();
}

void KTListVars::create()
{
  CTVariable* tempvar = new CTVariable();
  KTVariable* ktvar = new KTVariable(tempvar);
  ktvar->exec();
  delete ktvar;
  if (tempvar->dirty())
  {
    getCTCron()->variable.push_back(tempvar);
    KTListVar* ktlv = new KTListVar(this, getCTCron(), tempvar);
    setSelected(false);
    ktlv->setSelected(true);
  }
  else
  {
    delete tempvar;
  }
}

void KTListVars::refresh()
{
  setText(0, getDescription());
  setPixmap(0, KTIcon::variables(true));
}

QString KTListVars::getDescription()
{
  return i18n("Variables");
}
