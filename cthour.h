/***************************************************************************
 *   cthour.h                                                              *
 *   --------------------------------------------------------------------  *
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

#include "ctunit.h"
#include <string>

/**
  * Cron table task hour.
  */
class CTHour : public CTUnit<0,23>
{
public:
  CTHour(const string &tokStr = "") : CTUnit<0,23>(tokStr) { };
};

#endif // CTHOUR_H
