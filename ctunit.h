/***************************************************************************
 *   ctunit.h                                                              *
 *   --------------------------------------------------------------------  *
 *   CT Unit of Time Interval Header                                       *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTUNIT_H
#define CTUNIT_H

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include <string>
class ostream;

/**
  * A cron table unit parser and tokenizer.
  * Parses/tokenizes unit such as "0-3,5,6,10-30/5"
  * Also provides default natural language description.
  */
template<int min, int max> class CTUnit
{
public:

/**
  * Constant indicating short format.
  */
  static const bool shortFormat = 0;

/**
  * Constant indicating long format.
  */
  static const bool longFormat = 1;

/**
  * Initialize including parsing and saving initial image.
  */
  CTUnit(const string &tokStr = "");

/**
  * Base initial image as empty and copy enabled intervals.
  */
  CTUnit(const CTUnit& source);

/**
  * Destructor.
  */
  virtual ~CTUnit();

/**
  * Copy enabled intervals.
  */
  void operator = (const CTUnit<min,max>& unit);

/**
  * Returns tokenization to output stream.
  */
  friend ostream& operator << (ostream& outStr, const CTUnit<min,max>& unit)
  {
    outStr << unit.tokenize();
    return outStr;
  };

/**
  * Parses unit such as "0-3,5,6,10-30/5".
  * And initialize array of enabled intervals.
  */
  void initialize(const string &tokStr = "");

/**
  * Parses unit such as "0-3,5,6,10-30/5".
  * Does not initialize array of enabled intervals.
  */
  void parse(string tokStr = "");
  
/**
  * Tokenizes unit into string such as
  * "0,1,2,3,5,6,10,15,20,25,30".
  */
  string tokenize() const;

/**
  * Get default natural language description.
  */
  virtual string describe(const string *label) const;

/**
  * Lower bound.
  */
  int begin();

/**
  * Upper bound.
  */
  int end();

/**
  * Accessor.
  */
  bool get(int pos) const;

/**
  * Mutator.
  */
  void set(int pos, bool value);

/**
  * Enable.
  */
  void enable(int pos);

/**
  * Disable.
  */
  void disable(int pos);

/**
  * Indicates whether enabled intervals have been modified.
  */
  bool dirty() const;

/**
  * Total count of enabled intervals.
  */
  int count() const;

/**
  * Mark changes as applied.
  */
  void apply();

/**
  * Mark cancel changes and revert to initial or last applied
  * image.
  */
  void cancel();

private:

  int fieldToValue(string entry) const;
  bool isDirty;
  bool enabled[max+1];
  bool initialEnabled[max+1];
  string initialTokStr;

};

#include "ctunit.cpp"

#endif // CTUNIT_H
