/***************************************************************************
 *   ctdow.cpp                                                             *
 *   --------------------------------------------------------------------  *
 *   CT Day Of Week Implementation                                         *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctdow.h"
#include "cti18n.h"
#include <langinfo.h>

CTDayOfWeek::CTDayOfWeek(const string& tokStr) :
  CTUnit<1,7>(tokStr)
{
  // Note that langinfo.h constants for days of the week don't match
  // crontabs.

  const string shortDOWName[8] =
  {
    "",
    nl_langinfo(ABDAY_2),  nl_langinfo(ABDAY_3),  nl_langinfo(ABDAY_4),
    nl_langinfo(ABDAY_5),  nl_langinfo(ABDAY_6),  nl_langinfo(ABDAY_7),
    nl_langinfo(ABDAY_1)
  };

  const string longDOWName[8] =
  {
    "",
    nl_langinfo(DAY_2),    nl_langinfo(DAY_3),    nl_langinfo(DAY_4),
    nl_langinfo(DAY_5),    nl_langinfo(DAY_6),    nl_langinfo(DAY_7),
    nl_langinfo(DAY_1)
  };

  for (int i = 1; i <= 7; i++)
  {
    shortName[i] = shortDOWName[i];
    longName[i]  = longDOWName[i];
  }

  // Compensate for cron supporting Sunday as both 0 and 7.

  if (get(0))
  {
    set(0,false);
    set(7,true);
  }
}

void CTDayOfWeek::initialize(const string &tokStr)
{
  CTUnit<1,7>::initialize(tokStr);

  // Compensate for cron supporting Sunday as both 0 and 7.

  if (get(0))
  {
    set(0,false);
    set(7,true);
    apply();
  }
}

string CTDayOfWeek::describe() const
{
  if (count() == 7)
    return (const char*)i18n("every day ");
  else if (get(1) && get(2) && get(3) && get(4) && get(5))
    return (const char*)i18n("weekday ");
  else
    return CTUnit<1,7>::describe(shortName);
}

string CTDayOfWeek::getName(const int ndx, const bool format) const
{
  return (format == CTDayOfWeek::shortFormat) ?
    shortName[ndx] : longName[ndx];
}
