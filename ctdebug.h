/***************************************************************************
 *   ctdebug.h                                                             *
 *   --------------------------------------------------------------------  *
 *   CT Debug Header                                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTDEBUG_H
#define CTDEBUG_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include <iostream.h>
#include <string>

/**
  * Simple run-time entry/exit tracer.
  */

class CTDebug
{
private:
  string context;

public:
  CTDebug(string _context = "Undefined Context")
  {
    cout << (context = _context) << ": begin" << endl;
  };

  ~CTDebug()
  {
    cout << context << ": end" << endl;
  }
};

// #define CTDEBUG(X) CTDebug ctdebug(X);

#define CTDEBUG(X)

#endif // CTDEBUG_H
