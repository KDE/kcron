/***************************************************************************
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

#include <vector>
#include <string>

#include <qstring.h> // Whatever

class CTCron;
struct passwd;

/**
  * The host machine, or computer (encapsulation of crontab files on the
  * host).
  *
  * If the user is the root user, the cron vector will have a member for
  * each user of the host plus one for the system crontab.
  *
  * If the user is a non-root user, there will be only one member in the
  * cron vector.
  */
class CTHost
{
public:

/**
  * Constructs the user(s), scheduled tasks, and environment variables
  * from crontab files.
  */
  CTHost();

/**
  * Destroys the user(s), scheduled tasks, and environment variable
  * objects.  Does not make any changes to the crontab files.  Any unapplied
  * changes are consequently "cancelled."
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
  * Indicates whether or not the user is the root user.
  */
  bool root() const;

/**
  * Indicates an error has occured.
  */
  bool isError() { return !error.isEmpty(); }

/**
  * Error message
  */
  QString errorMessage() { QString r = error; error = QString::null; return r; }  


/**
  * User(s). 
  *
  * If the user is the root user, the cron vector will have a member for
  * each user of the host plus one for the system crontab.
  *
  * If the user is a non-root user, there will be only one member in the
  * cron vector.
  */
  std::vector<CTCron*> cron;

private:

/**
  * Copy construction not allowed.
  */
  CTHost(const CTHost& source);

/**
  * Assignment not allowed
  */
  void operator = (const CTHost& source);

/**
  * Factory create a cron table.  Appends to the end of cron.
  */
  CTCron* createCTCron(bool _syscron = false, std::string _login = "");
  CTCron* createCTCron(const struct passwd *);

  QString error;
};

typedef std::vector<CTCron*>::iterator CTCronIterator;

#endif // CTHOST_H
