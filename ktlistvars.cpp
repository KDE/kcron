/***************************************************************************
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

#include <qstring.h>

#include <klocale.h>  // i18n()
#include <kglobalsettings.h>

#include "ctcron.h"
#include "ctvariable.h"

#include "kticon.h"
#include "ktlistvar.h"
#include "ktvariable.h"
#include "ktprint.h"

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
  KTVariable* ktvar = new KTVariable(tempvar,i18n("Edit Variable"));
  ktvar->exec();
  delete ktvar;
  if (tempvar->dirty())
  {
    getCTCron()->variable.push_back(tempvar);
    new KTListVar(this, getCTCron(), tempvar); // Qt will clean up
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

void KTListVars::print(KTPrint& printer) const
{
  QFont stnd;

  stnd = printer.getFont() ;
  printer.setFont(QFont( KGlobalSettings::generalFont().family(), 10, QFont::Bold ));

  printer.print(i18n("Variable:"), 1, KTPrint::alignTextLeft);
  printer.print(i18n("Value:"), 2, KTPrint::alignTextCenter);
  printer.print(i18n("Description:"), 3, KTPrint::alignTextRight);

  printer.setFont(stnd);

  //firstChild() does not return null if there are no children, therefore
  //we need to check the validation of the pointer without terminating
  //the application. This maybe a bug in QT 1.44

  if (this->childCount() ==0) {
    printer.print(i18n("No variables..."),1,KTPrint::alignTextLeft, false);
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

QString KTListVars::getDescription()
{
  return i18n("Variables");
}
