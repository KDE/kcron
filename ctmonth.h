/***************************************************************************
 *   ctmonth.h                                                             *
 *   --------------------------------------------------------------------  *
 *   CT Month Header                                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTMONTH_H
#define CTMONTH_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctunit.h"
#include <string>

/**
  * Scheduled task months.
  */
class CTMonth : public CTUnit<1,12>
{
public:

/**
  * Constructs from a tokenized string.
  */
  CTMonth(const string& tokStr = "");

/**
  * Default copy constructor.
  */
  // CTMonth(const CTMonth& source);

/**
  * Default assignment operator
  */
  // void operator = (const CTMonth& source);

/**
  * Default destructor.
  */
  // ~CTMonth();

/**
  * Get natural language description.
  */
  virtual string describe() const;

/**
  * Get month name.
  */
  static string getName(const int ndx,
    const bool format = CTMonth::longFormat);

private:
  static void initializeNames();
  static string shortName[13];
  static string longName[13];
};

#endif // CTMONTH_H
