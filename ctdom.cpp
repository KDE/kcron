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

string CTDayOfMonth::shortName[32] =
{
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
  "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""
};

CTDayOfMonth::CTDayOfMonth(const string& tokStr) :
  CTUnit<1,31>(tokStr)
{
}

string CTDayOfMonth::describe() const
{
  return (count() == 31) ?
    (const char*)i18n("every day ") :
    CTUnit<1,31>::describe(shortName);
}

string CTDayOfMonth::getName(const int ndx)
{
  if (shortName[1] == "")
  {
    const string shortDOMName[32] =
    {
      "",
      (const char*)i18n("1st"),  (const char*)i18n("2nd"),
      (const char*)i18n("3rd"),  (const char*)i18n("4th"),
      (const char*)i18n("5th"),  (const char*)i18n("6th"),
      (const char*)i18n("7th"),  (const char*)i18n("8th"),
      (const char*)i18n("9th"),  (const char*)i18n("10th"),
      (const char*)i18n("11th"), (const char*)i18n("12th"),
      (const char*)i18n("13th"), (const char*)i18n("14th"),
      (const char*)i18n("15th"), (const char*)i18n("16th"), 
      (const char*)i18n("17th"), (const char*)i18n("18th"),
      (const char*)i18n("19th"), (const char*)i18n("20th"),
      (const char*)i18n("21st"), (const char*)i18n("22nd"),
      (const char*)i18n("23rd"), (const char*)i18n("24th"), 
      (const char*)i18n("25th"), (const char*)i18n("26th"),
      (const char*)i18n("27th"), (const char*)i18n("28th"), 
      (const char*)i18n("29th"), (const char*)i18n("30th"),
      (const char*)i18n("31st")
    };
  
    for (int i = 1; i <= 31; i++)
    {
      shortName[i] = shortDOMName[i];
    }
  }

  return shortName[ndx];
}
