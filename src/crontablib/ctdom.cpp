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
    (const char*)i18n("every day ").toLocal8Bit() :
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
      (const char*)i18n("1st").toLocal8Bit(),  (const char*)i18n("2nd").toLocal8Bit(),
      (const char*)i18n("3rd").toLocal8Bit(),  (const char*)i18n("4th").toLocal8Bit(),
      (const char*)i18n("5th").toLocal8Bit(),  (const char*)i18n("6th").toLocal8Bit(),
      (const char*)i18n("7th").toLocal8Bit(),  (const char*)i18n("8th").toLocal8Bit(),
      (const char*)i18n("9th").toLocal8Bit(),  (const char*)i18n("10th").toLocal8Bit(),
      (const char*)i18n("11th").toLocal8Bit(), (const char*)i18n("12th").toLocal8Bit(),
      (const char*)i18n("13th").toLocal8Bit(), (const char*)i18n("14th").toLocal8Bit(),
      (const char*)i18n("15th").toLocal8Bit(), (const char*)i18n("16th").toLocal8Bit(), 
      (const char*)i18n("17th").toLocal8Bit(), (const char*)i18n("18th").toLocal8Bit(),
      (const char*)i18n("19th").toLocal8Bit(), (const char*)i18n("20th").toLocal8Bit(),
      (const char*)i18n("21st").toLocal8Bit(), (const char*)i18n("22nd").toLocal8Bit(),
      (const char*)i18n("23rd").toLocal8Bit(), (const char*)i18n("24th").toLocal8Bit(), 
      (const char*)i18n("25th").toLocal8Bit(), (const char*)i18n("26th").toLocal8Bit(),
      (const char*)i18n("27th").toLocal8Bit(), (const char*)i18n("28th").toLocal8Bit(), 
      (const char*)i18n("29th").toLocal8Bit(), (const char*)i18n("30th").toLocal8Bit(),
      (const char*)i18n("31st").toLocal8Bit()
    };
  
    for (int i = 1; i <= 31; i++)
    {
      shortName[i] = shortDOMName[i];
    }
  }
}
