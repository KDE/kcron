/***************************************************************************
 *   ctvariable.h                                                          *
 *   --------------------------------------------------------------------  *
 *   CT Environment Variable Header                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTVARIABLE_H
#define CTVARIABLE_H

#include <string>
#include <iostream.h>

/**
  * Encapsulation of cron environment variable entry.
  */
class CTVariable
{
public:

/**
  * Constructs environment variable.
  */
  CTVariable(string tokStr = "", string _comment = "");

/**
  * Copy constructor.
  */
  CTVariable(const CTVariable& source);

/**
  * Assignment operator.
  */
  void operator = (const CTVariable& source);

/**
  * Tokenizes cron table entry to output stream.
  */
  friend ostream& operator << (ostream& outputStream, const CTVariable& task);

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

  string       variable;
  string       value;
  string       comment;
  bool         enabled;

private:
  string       initialVariable;
  string       initialValue;
  string       initialComment;
  bool         initialEnabled;

};

#endif // CTVARIABLE_H
