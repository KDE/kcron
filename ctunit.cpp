/***************************************************************************
 *   ctunit.cpp                                                            *
 *   --------------------------------------------------------------------  *
 *   CT Unit of Time Interval Implementation                               *
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

#include "cti18n.h"
#include <vector>
#include <string>
#include <stdio.h>    // sprintf
#include <ctype.h>    // tolower

using namespace std;

template<int min, int max>
CTUnit<min, max>::CTUnit(const string &tokStr)
{
  initialize(tokStr);
}

template<int min, int max>
CTUnit<min, max>::CTUnit(const CTUnit& source)
{
  for (int i = 0; i <= max; i++)
  {
    initialEnabled[i] = 0;
    enabled[i] = source.enabled[i];
  }
  initialTokStr = "";
  isDirty = true;
}

template<int min, int max>
CTUnit<min, max>::~CTUnit()
{
}

template<int min, int max>
void CTUnit<min, max>::operator = (const CTUnit<min,max>& unit)
{
  for (int i = 0; i <= max; i++)
    enabled[i] = unit.enabled[i];
  isDirty = true;
  return;
}

template<int min, int max>
void CTUnit<min, max>::initialize(const string &tokStr)
{
  for (int i = 0; i <= max; i++)
    enabled[i] = 0;

  parse(tokStr);

  for (int i = min; i <= max; i++)
    initialEnabled[i] = enabled[i];

  initialTokStr = tokStr;
  isDirty = false;

  return;
}

template<int min, int max>
void CTUnit<min, max>::parse(string tokStr)
{
  // subelement is that which is between commas
  string subelement;
  int commapos, slashpos, dashpos;
  int beginat, endat, step;

  // loop through each subelement
  tokStr += ",";
  while ((commapos = tokStr.find(",")) > 0)
  {
    subelement = tokStr.substr(0,commapos);

    // find "/" to determine step
    slashpos = subelement.find("/");
    if (slashpos == -1)
    {
      step = 1;
      slashpos = subelement.length();
    }
    else
      step = fieldToValue(subelement.substr(slashpos+1,
        subelement.length()-slashpos-1));

    // find "=" to determine range
    dashpos = subelement.find("-");
    if (dashpos == -1)
    {
      // deal with "*"
      if (subelement.substr(0,slashpos) == "*")
      {
        beginat = min;
        endat = max;
      }
      else
      {
        beginat = fieldToValue(subelement.substr(0,slashpos));
        endat = beginat;
      }
    }
    else
    {
      beginat = fieldToValue(subelement.substr(0,dashpos));
      endat = fieldToValue(subelement.substr(dashpos+1,slashpos-dashpos-1));
    }

    // ignore out of range
    if (beginat < 0)
      beginat = 0;
    if (endat > max)
      endat = max;

    // setup enabled
    for (int i = beginat; i <= endat; i+=step)
      enabled[i] = 1;

    tokStr = tokStr.substr(commapos+1, tokStr.length()-commapos-1);
  }
  return;
}

template<int min, int max>
string CTUnit<min, max>::tokenize() const
{
  if (!isDirty)
  {
    return initialTokStr;
  }
  else
  {
    int total(count());
    int count(0);
    int num(min);
    string tmpStr;

    while (num <= max)
    {
      if (enabled[num])
      {
        char cnum[3];
        sprintf(cnum, "%u", num);
        tmpStr += cnum;
        if (++count < total)
          tmpStr += ",";
      }
      num++;
    }
    if (count == max)
      tmpStr = "*";
    return tmpStr;
  }
}

template<int min, int max>
string CTUnit<min, max>::describe(const string *label) const
{
  int total(count());
  int count(0);
  string tmpStr;
  for (int i = min; i <= max; i++)
  {
    if (enabled[i])
    {
      tmpStr += label[i];
      count++;
      switch (total - count)
      {
        case 0:  break;
        case 1:  if (total > 2) tmpStr += (const char *)i18n(",").local8Bit();
                 tmpStr += (const char *)i18n(" and ").local8Bit();
                 break;
        default: tmpStr += (const char *)i18n(", ").local8Bit();
                 break;
      }
    }
  }
  return tmpStr;
}

template<int min, int max>
int CTUnit<min, max>::begin()
{
  return min;
}

template<int min, int max>
int CTUnit<min, max>::end()
{
  return max;
}

template<int min, int max>
bool CTUnit<min, max>::get(int pos) const
{
  return enabled[pos];
}

template<int min, int max>
void CTUnit<min, max>::set(int pos, bool value)
{
  enabled[pos] = value;
  isDirty = true;
  return;
}

template<int min, int max>
void CTUnit<min, max>::enable(int pos)
{
  enabled[pos] = true;
  isDirty = true;
  return;
}

template<int min, int max>
void CTUnit<min, max>::disable(int pos)
{
  enabled[pos] = false;
  isDirty = true;
  return;
}

template<int min, int max>
bool CTUnit<min, max>::dirty() const
{
  return isDirty;
}

template<int min, int max>
int CTUnit<min, max>::count() const
{
  int total(0);
  for (int i = min; i <= max; i++)
    total += (enabled[i] == true);
  return total;
}

template<int min, int max>
void CTUnit<min, max>::apply()
{
  initialTokStr = tokenize();
  for (int i = min; i <= max; i++)
    initialEnabled[i] = enabled[i];
  isDirty = false;
  return;
}

template<int min, int max>
void CTUnit<min, max>::cancel()
{
  for (int i = min; i <= max; i++)
    enabled[i] = initialEnabled[i];
  isDirty = false;
  return;
}

template<int min, int max>
int CTUnit<min, max>::fieldToValue(string entry) const
{
  // GNU C++ STL has no String::toLower() so we have to lower
  // by hand.

  string lower("");
  int length = entry.length();
  for (int i = 0; i < length; i++)
    lower += tolower(*(entry.substr(i, 1).c_str()));

  // check for days
  string days[7] =
  {
    "sun", "mon", "tue", "wed", "thu", "fri", "sat"
  };

  for (int day = 0; day < 7; day++)
  {
    if (lower == days[day])
    {
      char cnum[3];
      sprintf(cnum, "%u", day);
      entry = cnum;
    }
  }

  // check for months
  string months[13] =
  {
    "",
    "jan", "feb", "mar", "apr", "may", "jun",
    "jul", "aug", "sep", "oct", "nov", "dec"
  };

  for (int month = 1; month < 13; month++)
  {
    if (lower == months[month])
    {
      char cnum[3];
      sprintf(cnum, "%u", month);
      entry = cnum;
    }
  }

  return atoi(entry.c_str());
}
