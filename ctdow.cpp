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

string CTDayOfWeek::shortName[8] =
{
  "", "", "", "", "", "", "", ""
};

string CTDayOfWeek::longName[8] =
{
  "", "", "", "", "", "", "", ""
};

CTDayOfWeek::CTDayOfWeek(const string& tokStr) :
  CTUnit<1,7>(tokStr)
{
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
  initializeNames();
  if (count() == 7)
    return (const char*)i18n("every day ");
  else if (get(1) && get(2) && get(3) && get(4) && get(5))
    return (const char*)i18n("weekday ");
  else
    return CTUnit<1,7>::describe(shortName);
}

string CTDayOfWeek::getName(const int ndx, const bool format)
{
  initializeNames();
  return (format == shortFormat) ? shortName[ndx] : longName[ndx];
}

void CTDayOfWeek::initializeNames()
{
  if (shortName[1] == "")
  {
    const string shortDOWName[8] =
    {
      "",
      (const char*)i18n("Mon"),  (const char*)i18n("Tue"),
      (const char*)i18n("Wed"),  (const char*)i18n("Thu"),
      (const char*)i18n("Fri"),  (const char*)i18n("Sat"),
      (const char*)i18n("Sun")
    };
  
    const string longDOWName[8] =
    {
      "",
      (const char*)i18n("Monday"),     (const char*)i18n("Tuesday"),
      (const char*)i18n("Wednesday"),  (const char*)i18n("Thursday"),
      (const char*)i18n("Friday"),     (const char*)i18n("Saturday"),
      (const char*)i18n("Sunday")
    };
  
    for (int i = 1; i <= 7; i++)
    {
      shortName[i] = shortDOWName[i];
      longName[i]  = longDOWName[i];
    }
  }
}
