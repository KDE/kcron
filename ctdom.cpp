/***************************************************************************
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
 initializeNames();
  return (count() == 31) ?
    (const char*)i18n("every day ").local8Bit() :
    CTUnit<1,31>::describe(shortName);
}

string CTDayOfMonth::getName(const int ndx)
{
  initializeNames();
  return shortName[ndx];
}

void CTDayOfMonth::initializeNames()
{
  if (shortName[1].empty())
  {
    const string shortDOMName[32] =
    {
      "",
      (const char*)i18n("1st").local8Bit(),  (const char*)i18n("2nd").local8Bit(),
      (const char*)i18n("3rd").local8Bit(),  (const char*)i18n("4th").local8Bit(),
      (const char*)i18n("5th").local8Bit(),  (const char*)i18n("6th").local8Bit(),
      (const char*)i18n("7th").local8Bit(),  (const char*)i18n("8th").local8Bit(),
      (const char*)i18n("9th").local8Bit(),  (const char*)i18n("10th").local8Bit(),
      (const char*)i18n("11th").local8Bit(), (const char*)i18n("12th").local8Bit(),
      (const char*)i18n("13th").local8Bit(), (const char*)i18n("14th").local8Bit(),
      (const char*)i18n("15th").local8Bit(), (const char*)i18n("16th").local8Bit(), 
      (const char*)i18n("17th").local8Bit(), (const char*)i18n("18th").local8Bit(),
      (const char*)i18n("19th").local8Bit(), (const char*)i18n("20th").local8Bit(),
      (const char*)i18n("21st").local8Bit(), (const char*)i18n("22nd").local8Bit(),
      (const char*)i18n("23rd").local8Bit(), (const char*)i18n("24th").local8Bit(), 
      (const char*)i18n("25th").local8Bit(), (const char*)i18n("26th").local8Bit(),
      (const char*)i18n("27th").local8Bit(), (const char*)i18n("28th").local8Bit(), 
      (const char*)i18n("29th").local8Bit(), (const char*)i18n("30th").local8Bit(),
      (const char*)i18n("31st").local8Bit()
    };
  
    for (int i = 1; i <= 31; i++)
    {
      shortName[i] = shortDOMName[i];
    }
  }
}
