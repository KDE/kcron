/***************************************************************************
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

string CTMonth::shortName[13] =
{
  "", "", "", "", "", "", "", "", "", "", "", "", "",
};

CTMonth::CTMonth(const string& tokStr) :
  CTUnit<1,12>(tokStr)
{
}

string CTMonth::describe () const
{
  initializeNames();
  return (count() == 12) ?
    (const char*)i18n("every month ").local8Bit() :
    CTUnit<1,12>::describe(shortName);
}

string CTMonth::getName(const int ndx)
{
  initializeNames();
  return shortName[ndx];
}

void CTMonth::initializeNames()
{
  if (shortName[1].empty())
  {
    const string shortMonthName[13] =
    {
      "",
      (const char*)i18n("January").local8Bit(),  (const char*)i18n("February").local8Bit(),
      (const char*)i18n("March").local8Bit(),  (const char*)i18n("April").local8Bit(),
      (const char*)i18n("May long","May").local8Bit(),  (const char*)i18n("June").local8Bit(),
      (const char*)i18n("July").local8Bit(),  (const char*)i18n("August").local8Bit(),
      (const char*)i18n("September").local8Bit(),  (const char*)i18n("October").local8Bit(),
      (const char*)i18n("November").local8Bit(),  (const char*)i18n("December").local8Bit()
    };
  
    for (int i = 1; i <= 12; i++)
    {
      shortName[i] = shortMonthName[i];
    }
  }
}
