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

#include "ctunit.h"
#include <string>

/**
  * Cron table task day of month.
  */
class CTDayOfMonth : public CTUnit<1,31>
{
public:
/**
  * Constructs from a tokenized string.
  */
  CTDayOfMonth(const string& tokStr = "");

/**
  * Get natural language description.
  */
  string describe() const;

/**
  * Get day of month name.
  */
  string getName(const int ndx,
    const bool format = CTDayOfMonth::longFormat) const;

private:
  string shortName[32];
  string longName[32];
};

#endif // CTDOM_H
