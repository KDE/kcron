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
    cti18n("1st"),  cti18n("2nd"),  cti18n("3rd"),  cti18n("4th"),
    cti18n("5th"),  cti18n("6th"),  cti18n("7th"),  cti18n("8th"),
    cti18n("9th"),  cti18n("10th"), cti18n("11th"), cti18n("12th"),
    cti18n("13th"), cti18n("14th"), cti18n("15th"), cti18n("16th"), 
    cti18n("17th"), cti18n("18th"), cti18n("19th"), cti18n("20th"),
    cti18n("21st"), cti18n("22nd"), cti18n("23rd"), cti18n("24th"), 
    cti18n("25th"), cti18n("26th"), cti18n("27th"), cti18n("28th"), 
    cti18n("29th"), cti18n("30th"), cti18n("31st")
  };

  const string longDOMName[32] =
  {
    "",
    cti18n("First"),         cti18n("Second"),       cti18n("Third"),
    cti18n("Fourth"),        cti18n("Fifth"),        cti18n("Sixth"),
    cti18n("Seventh"),       cti18n("Eighth"),       cti18n("Ninth"),
    cti18n("Tenth"),         cti18n("Eleventh"),     cti18n("Twelfth"),
    cti18n("Thirteenth"),    cti18n("Fourteenth"),   cti18n("Fifteenth"),
    cti18n("Sixteenth"),     cti18n("Seventeenth"),  cti18n("Eighteenth"),
    cti18n("Ninteenth"),     cti18n("Twentieth"),    cti18n("Twenty-First"),
    cti18n("Twenty-Second"), cti18n("Twenty-Third"), cti18n("Twenty-Fourth"),
    cti18n("Twenty-Fifth"),  cti18n("Twenty-Sixth"), cti18n("Twenty-Seventh"),
    cti18n("Twenty-Eighth"), cti18n("Twenty-Ninth"), cti18n("Thirtieth"),
    cti18n("Thirtieth-First")
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
    cti18n("every day ") :
    CTUnit<1,31>::describe(shortName);
}

string CTDayOfMonth::getName(const int ndx, const bool format) const
{
  return (format == CTDayOfMonth::shortFormat) ?
    shortName[ndx] : longName[ndx];
}
