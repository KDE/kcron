/***************************************************************************
 *   ctdom.cpp                                                             *
 *   --------------------------------------------------------------------  *
 *   CT Day of Month Implementation                                        *
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

#include "ctdom.h"
#include "cti18n.h"

CTDayOfMonth::CTDayOfMonth(const string& tokStr) :
  CTUnit<1,31>(tokStr)
{
  const string shortDOMName[32] =
  {
    "",
    (const char*)i18n("1st"),  (const char*)i18n("2nd"),  (const char*)(const char*)i18n("3rd"),  (const char*)i18n("4th"),
    (const char*)i18n("5th"),  (const char*)i18n("6th"),  (const char*)i18n("7th"),  (const char*)i18n("8th"),
    (const char*)i18n("9th"),  (const char*)i18n("10th"), (const char*)i18n("11th"), (const char*)i18n("12th"),
    (const char*)i18n("13th"), (const char*)i18n("14th"), (const char*)i18n("15th"), (const char*)i18n("16th"), 
    (const char*)i18n("17th"), (const char*)i18n("18th"), (const char*)i18n("19th"), (const char*)i18n("20th"),
    (const char*)i18n("21st"), (const char*)i18n("22nd"), (const char*)i18n("23rd"), (const char*)i18n("24th"), 
    (const char*)i18n("25th"), (const char*)i18n("26th"), (const char*)i18n("27th"), (const char*)i18n("28th"), 
    (const char*)i18n("29th"), (const char*)i18n("30th"), (const char*)i18n("31st")
  };

  const string longDOMName[32] =
  {
    "",
    (const char*)i18n("First"),         (const char*)i18n("Second"),       (const char*)i18n("Third"),
    (const char*)i18n("Fourth"),        (const char*)i18n("Fifth"),        (const char*)i18n("Sixth"),
    (const char*)i18n("Seventh"),       (const char*)i18n("Eighth"),       (const char*)i18n("Ninth"),
    (const char*)i18n("Tenth"),         (const char*)i18n("Eleventh"),     (const char*)i18n("Twelfth"),
    (const char*)i18n("Thirteenth"),    (const char*)i18n("Fourteenth"),   (const char*)i18n("Fifteenth"),
    (const char*)i18n("Sixteenth"),     (const char*)i18n("Seventeenth"),  (const char*)i18n("Eighteenth"),
    (const char*)i18n("Ninteenth"),     (const char*)i18n("Twentieth"),    (const char*)i18n("Twenty-First"),
    (const char*)i18n("Twenty-Second"), (const char*)i18n("Twenty-Third"), (const char*)i18n("Twenty-Fourth"),
    (const char*)i18n("Twenty-Fifth"),  (const char*)i18n("Twenty-Sixth"), (const char*)i18n("Twenty-Seventh"),
    (const char*)i18n("Twenty-Eighth"), (const char*)i18n("Twenty-Ninth"), (const char*)i18n("Thirtieth"),
    (const char*)i18n("Thirtieth-First")
  };

  for (int i = 1; i <= 31; i++)
  {
    shortName[i] = shortDOMName[i];
    longName[i]  = longDOMName[i];
  }
}

string CTDayOfMonth::describe() const
{
  return (count() == 31) ?
    (const char*)i18n("every day ") :
    CTUnit<1,31>::describe(shortName);
}

string CTDayOfMonth::getName(const int ndx, const bool format) const
{
  return (format == CTDayOfMonth::shortFormat) ?
    shortName[ndx] : longName[ndx];
}
