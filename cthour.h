/***************************************************************************
 *   CT Hour Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTHOUR_H
#define CTHOUR_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctunit.h"
#include <string>

/**
  * Scheduled task hours.
  */
class CTHour : public CTUnit<0,23>
{
public:

/**
  * Constructs from a tokenized string.
  */
  CTHour(const string &tokStr = "") : CTUnit<0,23>(tokStr) { };

/**
  * Default copy constructor.
  */
  // CTHour(const CTHour& source);

/**
  * Default assignment operator
  */
  // void operator = (const CTHour& source);

/**
  * Default destructor.
  */
  // ~CTHour();

};

#endif // CTHOUR_H
