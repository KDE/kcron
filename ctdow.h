/***************************************************************************
 *   ctdow.h                                                               *
 *   --------------------------------------------------------------------  *
 *   CT Day of Week Header                                                 *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTDOW_H
#define CTDOW_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctunit.h"
#include <string>

/**
  * Scheduled task days of week.
  */
class CTDayOfWeek : public CTUnit<1,7>
{
public:
/**
  * Constructs from a tokenized string.
  */
  CTDayOfWeek(const string& tokStr = "");

/**
  * Default copy constructor.
  */
  // CTDayOfWeek(const CTDayOfWeek& source);

/**
  * Default assignment operator
  */
  // void operator = (const CTDayOfWeek& source);

/**
  * Default destructor.
  */
  // ~CTDayOfWeek();

/**
  * Override initialize to support crontab using both 0 and 7 for
  * Sunday.
  */
  void initialize(const string &tokStr = "");

/**
  * Get natural language description.
  */
  string describe() const;

/**
  * Get day of week name.
  */
  string getName(const int ndx,
    const bool format = CTDayOfWeek::longFormat) const;

private:
  string shortName[8];
  string longName[8];
};

#endif // CTDOW_H
