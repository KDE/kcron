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

#include <vector>
#include <string>
#include <iostream>

#include <qstring.h> // Anarchy! -WABA 

class CTException;
class CTTask;
class CTVariable;

/**
  * A user (encapsulation of a single crontab file).  Encapsulates
  * file i/o, parsing, tokenization, and natural language description.
  */
class CTCron
{
public:

/**
  * Constructs the scheduled tasks, environment variables from crontab
  * files and obtains some information about the user from the system.
  *
  * Default is to construct from the user's crontab.  Can also be called,
  * passing TRUE, to construct from the system crontab.  Throws an
  * exception if the crontab file can not be found, read, or parsed.
  */
  CTCron(bool _syscron = false, std::string _login = "");

/**
  * Copy one user's tasks and environement variables to another user.
  */
  void operator = (const CTCron& source);

/**
  * Parses crontab file format.
  */
  friend std::istream& operator >> (std::istream& inputStream, CTCron& cron);

/**
  * Tokenizes to crontab file format.
  */
  friend std::ostream& operator << (std::ostream& outputStream, const CTCron& cron);

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
  * Returns the PATH environment variable value.  A short cut to iterating
  * the tasks vector.
  */
  std::string path() const;

  /**
   * Returns whether an error has occured
   */
  bool isError() { return !error.isEmpty(); }
  
  /**
   * Return error description
   */
  QString errorMessage() { QString r = error; error = QString::null; return r; }  
   

/**
  * Indicates whether or not the crontab belongs to the system.
  */
  const bool syscron;

/**
  * User  login.
  */
  std::string login;

/**
  * User real name.
  */
  std::string name;

/**
  * User's scheduled tasks.
  */
  std::vector<CTTask *> task;

/**
  * User's environment variables.  Note:  These are only environment variables
  * found in the user's crontab file and does not include any set in a 
  * login or shell script such as ".bash_profile".
  */
  std::vector<CTVariable *> variable;

/**
  * Destructor.
  */
  ~CTCron();

private:

/**
  * Can't copy a user!
  */
  CTCron(const CTCron& source);

  unsigned int initialTaskCount;
  unsigned int initialVariableCount;
  std::string       writeCommand;
  std::string       tmpFileName;

  QString           error;
};

typedef std::vector<CTTask*>::iterator CTTaskIterator;
typedef std::vector<CTVariable*>::iterator CTVariableIterator;

#endif // CTCRON_H
