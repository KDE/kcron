/***************************************************************************
 *   cttask.h                                                              *
 *   --------------------------------------------------------------------  *
 *   CT Task Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTTASK_H
#define CTTASK_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include <string>

#include "ctmonth.h"
#include "ctdom.h"
#include "ctdow.h"
#include "cthour.h"
#include "ctminute.h"
#include <iostream.h> // istream, ostream

/**
  * A scheduled task (encapsulation of crontab entry).  Encapsulates
  * parsing, tokenization, and natural language description.
  */
class CTTask
{
public:

/**
  * Constructs scheduled task from crontab format string.
  */
  CTTask(string tokStr = "", string _comment = "", bool syscron = false);

/**
  * Copy constructor.
  */
  CTTask(const CTTask& source);

/**
  * Assignment operator.
  */
  void operator = (const CTTask& source);

/**
  * Default constructor.
  */
  // ~CTTask();

/**
  * Tokenizes scheduled task to crontab format.
  */
  friend ostream& operator << (ostream& outputStream, const CTTask& task);

/**
  * Mark changes as applied.
  */
  void apply();

/**
  * Cancel changes.
  */
  void cancel();

/**
  * Indicates whether or not the task has been modified.
  */
  bool dirty() const;

/**
  * Returns natural language description of the task's schedule.
  */
  string describe(bool format = 0) const;

/**
  * Indicates whether or not the task belongs to the system crontab.
  */
  bool system() const;

  CTMonth      month;
  CTDayOfMonth dayOfMonth;
  CTDayOfWeek  dayOfWeek;
  CTHour       hour;
  CTMinute     minute;
  string       user;
  string       command;
  string       comment;
  bool         enabled;

private:
  bool         syscron;
  string       initialUser;
  string       initialCommand;
  string       initialComment;
  bool         initialEnabled;

};

#endif // CTTASK_H
