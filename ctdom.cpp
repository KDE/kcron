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

#include "ctdom.h"
#include "cti18n.h"

CTDayOfMonth::CTDayOfMonth(const string& tokStr) :
  CTUnit<1,31>(tokStr)
{
  const string shortDOMName[32] =
  {
    "",
    i18n("1st"),  i18n("2nd"),  i18n("3rd"),  i18n("4th"),
    i18n("5th"),  i18n("6th"),  i18n("7th"),  i18n("8th"),
    i18n("9th"),  i18n("10th"), i18n("11th"), i18n("12th"),
    i18n("13th"), i18n("14th"), i18n("15th"), i18n("16th"), 
    i18n("17th"), i18n("18th"), i18n("19th"), i18n("20th"),
    i18n("21st"), i18n("22nd"), i18n("23rd"), i18n("24th"), 
    i18n("25th"), i18n("26th"), i18n("27th"), i18n("28th"), 
    i18n("29th"), i18n("30th"), i18n("31st")
  };

  const string longDOMName[32] =
  {
    "",
    i18n("First"),         i18n("Second"),       i18n("Third"),
    i18n("Fourth"),        i18n("Fifth"),        i18n("Sixth"),
    i18n("Seventh"),       i18n("Eighth"),       i18n("Ninth"),
    i18n("Tenth"),         i18n("Eleventh"),     i18n("Twelfth"),
    i18n("Thirteenth"),    i18n("Fourteenth"),   i18n("Fifteenth"),
    i18n("Sixteenth"),     i18n("Seventeenth"),  i18n("Eighteenth"),
    i18n("Ninteenth"),     i18n("Twentieth"),    i18n("Twenty-First"),
    i18n("Twenty-Second"), i18n("Twenty-Third"), i18n("Twenty-Fourth"),
    i18n("Twenty-Fifth"),  i18n("Twenty-Sixth"), i18n("Twenty-Seventh"),
    i18n("Twenty-Eighth"), i18n("Twenty-Ninth"), i18n("Thirtieth"),
    i18n("Thirtieth-First")
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
    i18n("every day ") :
    CTUnit<1,31>::describe(shortName);
}

string CTDayOfMonth::getName(const int ndx, const bool format) const
{
  return (format == CTDayOfMonth::shortFormat) ?
    shortName[ndx] : longName[ndx];
}
