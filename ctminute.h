/***************************************************************************
 *   ctminute.h                                                            *
 *   --------------------------------------------------------------------  *
 *   CT Minute Header                                                      *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTMINUTE_H
#define CTMINUTE_H

#include "ctunit.h"
#include <string>

/**
  * Cron table task minute.
  */
class CTMinute : public CTUnit<0,59>
{
public:
  CTMinute(const string &tokStr = "") : CTUnit<0,59>(tokStr) { };
};

#endif // CTMINUTE_H
