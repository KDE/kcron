/***************************************************************************
 *   ctdom.h                                                               *
 *   --------------------------------------------------------------------  *
 *   CT Day of Month Header                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTDOM_H
#define CTDOM_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctunit.h"
#include <string>

/**
  * Scheduled task days of month.
  */
class CTDayOfMonth : public CTUnit<1,31>
{
public:
/**
  * Constructs from a tokenized string.
  */
  CTDayOfMonth(const string& tokStr = "");

/**
  * Default copy constructor.
  */
  // CTDayOfMonth(const CTDayOfMonth& source);

/**
  * Default assignment operator
  */
  // void operator = (const CTDayOfMonth& source);

/**
  * Default destructor.
  */
  // ~CTDayOfMonth();

/**
  * Get natural language description.
  */
  string describe() const;

/**
  * Get day of month name.
  */
  string getName(const int ndx) const;

private:
  string shortName[32];
};

#endif // CTDOM_H
