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
    (const char*)i18n("every month ") :
    CTUnit<1,12>::describe(shortName);
}

string CTMonth::getName(const int ndx)
{
  initializeNames();
  return shortName[ndx];
}

void CTMonth::initializeNames()
{
  if (shortName[1] == "")
  {
    const string shortMonthName[13] =
    {
      "",
      (const char*)i18n("Jan"),  (const char*)i18n("Feb"),
      (const char*)i18n("Mar"),  (const char*)i18n("Apr"),
      (const char*)i18n("May"),  (const char*)i18n("Jun"),
      (const char*)i18n("Jul"),  (const char*)i18n("Aug"),
      (const char*)i18n("Sep"),  (const char*)i18n("Oct"),
      (const char*)i18n("Nov"),  (const char*)i18n("Dec")
    };
  
    for (int i = 1; i <= 12; i++)
    {
      shortName[i] = shortMonthName[i];
    }
  }
}
