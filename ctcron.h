/***************************************************************************
 *   ctcron.h                                                              *
 *   --------------------------------------------------------------------  *
 *   CT Cron Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTCRON_H
#define CTCRON_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "ctexception.h"
#include "cttask.h"
#include "ctvariable.h"
#include <vector.h>
#include <iostream.h>

/**
  * Encapsulation of the user's cron table.  Encapsulates
  * file i/o, parsing, tokenization, and natural language
  * description.
  */
class CTCron
{
public:

/**
  * Constructs the cron table.  Default is to construct
  * the user's cron table.  Can be called, passing TRUE,
  * to create the system cron table.  Throws an exception
  * if the cron table can not be read or parsed.
  */
  CTCron(bool _syscron = false, string _login = "");

/**
  * Invalid.  Throws exception.
  */
  CTCron(const CTCron& source);

/**
  * Copy data members except for login and name.
  */
  void operator = (const CTCron& source);

/**
  * Parses cron table from input stream.
  */
  friend istream& operator >> (istream& inputStream, CTCron& cron);

/**
  * Tokenizes cron table to output stream.
  */
  friend ostream& operator << (ostream& outputStream, const CTCron& cron);

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
  * Returns the PATH environment variable value.
  */
  string path() const;

/**
  * Indicates whether or not the cron table is the system
  * cron table.
  */
  const bool syscron;

/**
  * Account login.
  */
  string login;

/**
  * Account real name.
  */
  string name;

/**
  * Collection of scheduled tasks.
  */
  vector<CTTask *> task;

/**
  * Collection of environment variables set.
  */
  vector<CTVariable *> variable;

/**
  * Destructor.
  */
  ~CTCron();

private:

  unsigned int initialTaskCount;
  unsigned int initialVariableCount;
  string writeCommand;
  string tmpFileName;

};

typedef vector<CTTask*>::iterator CTTaskIterator;
typedef vector<CTVariable*>::iterator CTVariableIterator;

#endif // CTCRON_H
