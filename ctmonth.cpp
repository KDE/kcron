/***************************************************************************
 *   ctmonth.cpp                                                           *
 *   --------------------------------------------------------------------  *
 *   CT Month Implementation                                               *
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

#include "ctmonth.h"
#include "cti18n.h"
#include <langinfo.h>  // nl_langinfo()

CTMonth::CTMonth(const string& tokStr) :
  CTUnit<1,12>(tokStr)
{
  const string shortMonthName[13] =
  {
    "",
    nl_langinfo(ABMON_1),  nl_langinfo(ABMON_2),  nl_langinfo(ABMON_3),
    nl_langinfo(ABMON_4),  nl_langinfo(ABMON_5),  nl_langinfo(ABMON_6),
    nl_langinfo(ABMON_7),  nl_langinfo(ABMON_8),  nl_langinfo(ABMON_9),
    nl_langinfo(ABMON_10), nl_langinfo(ABMON_11), nl_langinfo(ABMON_12)
  };

  const string longMonthName[13] =
  {
    "",
    nl_langinfo(MON_1),    nl_langinfo(MON_2),    nl_langinfo(MON_3),
    nl_langinfo(MON_4),    nl_langinfo(MON_5),    nl_langinfo(MON_6),
    nl_langinfo(MON_7),    nl_langinfo(MON_8),    nl_langinfo(MON_9),
    nl_langinfo(MON_10),   nl_langinfo(MON_11),   nl_langinfo(MON_12)
  };

  for (int i = 1; i <= 12; i++)
  {
    shortName[i] = shortMonthName[i];
    longName[i]  = longMonthName[i];
  }
}

string CTMonth::describe () const
{
  return (count() == 12) ?
    (const char*)i18n("every month ") :
    CTUnit<1,12>::describe(shortName);
}

string CTMonth::getName(const int ndx, const bool format) const
{
  return (format == CTMonth::shortFormat) ?
    shortName[ndx] : longName[ndx];
}
