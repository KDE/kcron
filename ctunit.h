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

#include "ctdebug.h"
#include "cti18n.h"
#include <vector.h>
#include <string>
#include <iostream.h>
#include <stdio.h>

// Quick string to integer
#define S2I(X) atoi((string(X).c_str()));

/**
  * A cron table unit parser and tokenizer.
  * Parses/tokenizes unit such as "0-3,5,6,10-30/5"
  * Also provides default natural language description.
  */
template<int min, int max> class CTUnit
{
private:

/**
  * Indicates clean or dirty.
  */
  bool isDirty;

/**
  * Current enabled intervals.
  */
  // vector<bool> enabled;
  bool enabled[max+1];

/**
  * Initial enabled intervals or since last applied.
  */
  // vector<bool> initialEnabled;
  bool initialEnabled[max+1];

/**
  * Initial tokenization or since last applied.
  */
  string initialTokStr;

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
  * Lower bound.
  */
  int begin()
  {
    return min;
  }

/**
  * Upper bound.
  */
  int end()
  {
    return max;
  }

/**
  * Accessor.
  */
  bool get(int pos) const
  {
    return enabled[pos];
  }

/**
  * Mutator.
  */
  void set(int pos, bool value)
  {
    enabled[pos] = value;
    isDirty = true;
    return;
  }

/**
  * Enable.
  */
  void enable(int pos)
  {
    enabled[pos] = true;
    isDirty = true;
    return;
  }

/**
  * Disable.
  */
  void disable(int pos)
  {
    enabled[pos] = false;
    isDirty = true;
    return;
  }

/**
  * Indicates whether enabled intervals have been modified.
  */
  bool dirty() const
  {
    return isDirty;
  }

/**
  * Total count of enabled intervals.
  */
  int count() const
  {
    int total(0);
    for (int i = min; i <= max; i++)
      total += (enabled[i] == true);
    return total;
  }

/**
  * Parses unit such as "0-3,5,6,10-30/5"
  * Does not initialize array of enabled intervals.
  */
  void parse(string tokStr = "")
  {

    // subelement is that which is between commas
    string subelement;
    int commapos, slashpos, dashpos;
    int beginat, endat, step;
  
    // loop through each subelement
    tokStr += ",";
    while ((commapos = tokStr.find(",")) > 0)
    {
      subelement = tokStr.substr(0,commapos);
  
      // find "/" to determine step
      slashpos = subelement.find("/");
      if (slashpos == -1)
      {
        step = 1;
        slashpos = subelement.length();
      }
      else
        step = S2I(subelement.substr(slashpos+1,
          subelement.length()-slashpos-1));

      // find "=" to determine range
      dashpos = subelement.find("-");
      if (dashpos == -1)
        // deal with "*"
        if (subelement.substr(0,slashpos) == "*")
        {
          beginat = min;
          endat = max;
        }
        else
        {
          beginat = S2I(subelement.substr(0,slashpos));
          endat = beginat;
        }
      else
      {
        beginat = S2I(subelement.substr(0,dashpos));
        endat = S2I(subelement.substr(dashpos+1,slashpos-dashpos-1));
      }
  
      // ignore out of range
      if (beginat < 0)
        beginat = 0;
      if (endat > max)
        endat = max;
  
      // setup enabled
      for (int i = beginat; i <= endat; i+=step)
        enabled[i] = 1;

      tokStr = tokStr.substr(commapos+1, tokStr.length()-commapos-1);
    }
    return;
  }
  
/**
  * Tokenizes unit into string such as
  * "0,1,2,3,5,6,10,15,20,25,30"
  */
  string tokenize() const
  {
    CTDEBUG("CTUnit tokenize");

    if (!isDirty)
      return initialTokStr;
    else
    {
      int total(count());
      int count(0);
      int num(min);
      string tmpStr;

      while (num <= max)
      {
        if (enabled[num])
        {
          char cnum[3];
          sprintf(cnum, "%u", num);
          tmpStr += cnum;
          if (++count < total)
            tmpStr += ",";
        }
        num++;
      }
      return tmpStr;
    }
  };

/**
  * Get default natural language description.
  */
  string describe(const string *label) const
  {
    int total(count());
    int count(0);
    string tmpStr;
    for (int i = min; i <= max; i++)
      if (enabled[i])
      {
        tmpStr += label[i];
        count++;
        switch (total - count)
        {
          case 0:  break;
          case 1:  if (total > 2) tmpStr += i18n(",");
                   tmpStr += i18n(" and ");
                   break;
          default: tmpStr += i18n(", ");
                   break;
        }
      }
    return tmpStr;
  }

/**
  * Parses unit such as "0-3,5,6,10-30/5"
  * And initialize array of enabled intervals.
  */
  void initialize(const string &tokStr = "")
  {
    for (int i = min; i <= max; i++)
      enabled[i] = 0;

    parse(tokStr);

    for (int i = min; i <= max; i++)
      initialEnabled[i] = enabled[i];

    initialTokStr = tokStr;
    isDirty = false;

    return;
  }

/**
  * Initialize including parsing and saving initial image.
  */
  CTUnit(const string &tokStr = "")
  {
    initialize(tokStr);
  };

/**
  * Base initial image as empty and copy enabled intervals.
  */
  CTUnit(const CTUnit& source)
  {
    for (int i = min; i <= max; i++)
    {
      initialEnabled[i] = 0;
      enabled[i] = source.enabled[i];
    }
    initialTokStr = "";
    isDirty = true;
  };

/**
  * Copy enabled intervals.
  */
  // CTUnit<min,max>& operator = (const CTUnit<min,max>& unit)
  void operator = (const CTUnit<min,max>& unit)
  {
    for (int i = min; i <= max; i++)
      enabled[i] = unit.enabled[i];
    isDirty = true;
    // return *this;
    return;
  };

/**
  * Returns tokenization to output stream.
  */
  friend ostream& operator << (ostream& outStr, const CTUnit<min,max>& unit)
  {
    CTDEBUG("CTUnit extraction operator");
    outStr << unit.tokenize();
    return outStr;
  };

/**
  * Mark changes as applied.
  */
  void apply()
  {
    initialTokStr = tokenize();
    for (int i = min; i <= max; i++)
      initialEnabled[i] = enabled[i];
    isDirty = false;
    return;
  }

/**
  * Mark cancel changes and revert to initial or last applied
  * image.
  */
  void cancel()
  {
    for (int i = min; i <= max; i++)
      enabled[i] = initialEnabled[i];
    isDirty = false;
    return;
  }

/**
  * Destructor.
  */
  ~CTUnit() {};

/**
  * Dump guts for debugging.
  */
  void dump(const string& context = "Undefined") const
  {
    cout << "Begin dumping CTUnit from context: '" << context << "'" << endl;

    cout << "min: '" << min << "'" << endl;
    cout << "max: '" << max << "'" << endl;
    cout << "initialTokStr: '" << initialTokStr << "'" << endl;

    cout << "initialEnabled[]:" << endl;
    for (int i = min; i <= max; i++)
      cout << "\t" << i << "\t" << initialEnabled[i] << endl;

    cout << "enabled[]:" << endl;
    for (int i = min; i <= max; i++)
      cout << "\t" << i << "\t" << enabled[i] << endl;

    cout << "dirty(): '" << dirty() << "'" << endl;
    cout << "tokenize(): '" << tokenize() << "'" << endl;

    cout << "End dumping CTUnit from context: '" << context << "'" << endl;
  }

};

#endif // CTUNIT_H
