/***************************************************************************
 *   cthost.h                                                              *
 *   --------------------------------------------------------------------  *
 *   CT Host Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTHOST_H
#define CTHOST_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctexception.h"
#include "ctcron.h"
#include <vector.h>

/**
  * A singleton.  For the root user, encapsulates all users' cron tables
  * as well as the system cron table.  For non-root users, encapsulates
  * the user's only.
  */
class CTHost
{
public:

/**
  * Constructs the cron table(s).
  */
  CTHost();

/**
  * Destructs the cron table(s) canceling any unapplied changes.
  */
  ~CTHost();

/**
  * Apply changes.
  */
  void apply();

/**
  * Cancel changes.
  */
  void cancel();

/**
  * Indicates whether or not dirty.
  */
  bool dirty();

/**
  * Indicates whether or not the user is root user.
  */
  bool root() const;

/**
  * One for each account.
  */
  vector<CTCron*> cron;

private:

/**
  * Factory create a cron table.  Appends to the end of cron.
  */
  CTCron* createCTCron(bool _syscron = false, string _login = "");

};

typedef vector<CTCron*>::iterator CTCronIterator;

#endif // CTHOST_H
