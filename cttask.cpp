/***************************************************************************
 *   CT Task Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99

#include "cttask.h"

#include "cti18n.h"
#include <time.h>     // tm, strftime()

CTTask::CTTask(string tokStr, string _comment, bool _syscron) :
  syscron(_syscron)
{
  if (tokStr.substr(0,2) == "#\\")
  {
    tokStr = tokStr.substr(2,tokStr.length() - 2);
    enabled = false;
  }
  else if (tokStr.substr(0,1) == "#")
  {
    tokStr = tokStr.substr(1,tokStr.length() - 1);
    enabled = false;
  }
  else
    enabled = true;

  if (tokStr.substr(0,1) == "-")
  {
    tokStr = tokStr.substr(1,tokStr.length() - 1);
    silent = true;
  }
  else
  {
    silent = false;
  }
  
  if (tokStr.substr(0,1) == "@")
  {
    if (tokStr.substr(1,6) == "reboot")
    {
      // Dunno what to do with this...
      tokStr = "0 0 1 1 *"+tokStr.substr(7,tokStr.length()-1);
    }
    else if (tokStr.substr(1,6) == "yearly")
    {
      tokStr = "0 0 1 1 *"+tokStr.substr(7,tokStr.length()-1);
    }
    else if (tokStr.substr(1,8) == "annually")
    {
      tokStr = "0 0 1 1 *"+tokStr.substr(9,tokStr.length()-1);
    }
    else if (tokStr.substr(1,7) == "monthly")
    {
      tokStr = "0 0 1 * *"+tokStr.substr(8,tokStr.length()-1);
    }
    else if (tokStr.substr(1,6) == "weekly")
    {
      tokStr = "0 0 * * 0"+tokStr.substr(7,tokStr.length()-1);
    }
    else if (tokStr.substr(1,5) == "daily")
    {
      tokStr = "0 0 * * *"+tokStr.substr(6,tokStr.length()-1);
    }
    else if (tokStr.substr(1,6) == "hourly")
    {
      tokStr = "0 * * * *"+tokStr.substr(7,tokStr.length()-1);
    }
  }

  int spacepos(tokStr.find_first_of(" \t"));
  minute.initialize(tokStr.substr(0,spacepos));

  while(isspace(tokStr[spacepos+1])) spacepos++;
  tokStr     = tokStr.substr(spacepos+1,tokStr.length()-1);
  spacepos   = tokStr.find_first_of(" \t");
  hour.initialize(tokStr.substr(0,spacepos));

  while(isspace(tokStr[spacepos+1])) spacepos++;
  tokStr     = tokStr.substr(spacepos+1,tokStr.length()-1);
  spacepos   = tokStr.find_first_of(" \t");
  dayOfMonth.initialize(tokStr.substr(0,spacepos));

  while(isspace(tokStr[spacepos+1])) spacepos++;
  tokStr     = tokStr.substr(spacepos+1,tokStr.length()-1);
  spacepos   = tokStr.find_first_of(" \t");
  month.initialize(tokStr.substr(0,spacepos));

  while(isspace(tokStr[spacepos+1])) spacepos++;
  tokStr     = tokStr.substr(spacepos+1,tokStr.length()-1);
  spacepos   = tokStr.find_first_of(" \t");
  dayOfWeek.initialize(tokStr.substr(0,spacepos));

  if (syscron)
  {
    while(isspace(tokStr[spacepos+1])) spacepos++;
    tokStr     = tokStr.substr(spacepos+1,tokStr.length()-1);
    spacepos   = tokStr.find_first_of(" \t");
    user       = tokStr.substr(0,spacepos);
  }
  else
    user       = string("");

  command    = tokStr.substr(spacepos+1,tokStr.length()-1);
  // remove leading whitespace
  while (command.find_first_of(" \t") == 0)
     command = command.substr(1,command.length()-1);
  comment    = _comment;

  initialUser    = user;
  initialCommand = command;
  initialComment = comment;
  initialEnabled = enabled;
  initialSilent = silent;
}

CTTask::CTTask(const CTTask &source) :
  month(source.month),
  dayOfMonth(source.dayOfMonth),
  dayOfWeek(source.dayOfWeek),
  hour(source.hour),
  minute(source.minute),
  user(source.user),
  command(source.command),
  comment(source.comment),
  enabled(source.enabled),
  silent(source.silent),
  initialUser(""),
  initialCommand(""),
  initialComment(""),
  initialEnabled(true),
  initialSilent(false)
{
}

void CTTask::operator = (const CTTask& source)
{
  month          = source.month;
  dayOfMonth     = source.dayOfMonth;
  dayOfWeek      = source.dayOfWeek;
  hour           = source.hour;
  minute         = source.minute;
  user           = source.user;
  command        = source.command;
  comment        = source.comment;
  enabled        = source.enabled;
  silent         = source.silent;
  initialUser    = "";
  initialCommand = "";
  initialComment = "";
  initialEnabled = true;
  initialSilent  = false;
  return;
}

ostream& operator << (ostream& outputStream, const CTTask& task)
{
//  if (task.comment != string(""))
    outputStream << "# " << task.comment << "\n";

  if (!task.enabled)
    outputStream << "#\\";

  if (task.silent)
    outputStream << "-";

  outputStream << task.minute << " ";
  outputStream << task.hour << " ";
  outputStream << task.dayOfMonth << " ";
  outputStream << task.month << " ";
  outputStream << task.dayOfWeek << "\t";

  if (task.user != string(""))
    outputStream << task.user << "\t";

  outputStream << task.command << "\n";

  return outputStream;
}

void CTTask::apply()
{
  month.apply();
  dayOfMonth.apply();
  dayOfWeek.apply();
  hour.apply();
  minute.apply();
  initialUser    = user;
  initialCommand = command;
  initialComment = comment;
  initialEnabled = enabled;
  initialSilent = silent;
}

void CTTask::cancel()
{
  month.cancel();
  dayOfMonth.cancel();
  dayOfWeek.cancel();
  hour.cancel();
  minute.cancel();
  user    = initialUser;
  command = initialCommand;
  comment = initialComment;
  enabled = initialEnabled;
  silent = initialSilent;
}

bool CTTask::dirty() const
{
  return (month.dirty() || dayOfMonth.dirty() || dayOfWeek.dirty() ||
    hour.dirty() || minute.dirty() || (user != initialUser) ||
    (command != initialCommand) || (comment != initialComment) ||
    (enabled != initialEnabled) || (silent != initialSilent));
}

string CTTask::describe() const
{

  // Of the whole program, this method is probably the trickiest.
  //
  // This method creates the natural language description, such as
  // "At 1:00am, every Sun".
  //
  // First, I declare some strings for holding what can be internationalized.
  // Note the tokens such as "MONTHS".  Translators should reuse these
  // tokens in their translations.  See README.translators for more
  // information.
  //
  // Second, I get the descriptions from the component parts such as
  // days of the month.
  //
  // Third, I get hour/minute time combinations.  Although a little bit
  // awkward, I use the tm struct and strftime from <time.h>.  This
  // way this code is portable across all Unixes.
  //
  // Fourth, I know that "every day of the week" and "every day of the
  // month" simply makes "every day".
  //
  // Fifth and finally I do tag substitution to create the natural language
  // description.

  string tmFormat((const char *)i18n("%H:%M").local8Bit());
  string DOMFormat((const char *)i18n("Please translator, read the README.translators file in kcron's source code","DAYS_OF_MONTH of MONTHS").local8Bit());
  string DOWFormat((const char *)i18n("Really, read that file","every DAYS_OF_WEEK").local8Bit());
  string dateFormat((const char *)i18n("DOM_FORMAT as well as DOW_FORMAT").local8Bit());
  string timeFormat((const char *)i18n("At TIME").local8Bit());
  string format((const char *)i18n("TIME_FORMAT, DATE_FORMAT").local8Bit());

  // Get natural language description of day of month,
  // month name, and day of week.

  string DOMDesc(dayOfMonth.describe());
  string monthDesc(month.describe());
  string DOWDesc(dayOfWeek.describe());

  // Create time description.

  int total(minute.count()*hour.count());

  string timeDesc("");
  int count(0);

  for (int h = 0; h <= 23; h++)
    if (hour.get(h))
     for (int m = 0; m <= 59; m++)
       if (minute.get(m))
       {
         tm time;
         time.tm_sec  = 0;
         time.tm_min  = m;
         time.tm_hour = h;
         time.tm_mday = 0;
         time.tm_mon  = 0;
         time.tm_year = 0;
         time.tm_wday = 0;
         time.tm_yday = 0;
         time.tm_isdst= 0;

         char tmp[12];
         strftime(tmp, 12, tmFormat.c_str(), &time);
         string tmpStr = tmp;

         // remove leading space
         if (tmpStr.substr(0,1) == " ")
           tmpStr = tmpStr.substr(1,tmpStr.length()-1);

         timeDesc += tmpStr;
         count++;
         switch (total - count)
         {
           case 0:  break;
           case 1:  if (total > 2)
	   	      timeDesc += (const char *)i18n(", and ").local8Bit();
		    else
                      timeDesc += (const char *)i18n(" and ").local8Bit();
                    break;
           default: timeDesc += (const char*)i18n(", ").local8Bit();
         }
      }

  // "* * *" means truly every day.
  // Note: Languages may use different phrases to indicate
  // every day of month versus every day of week.

  if ((dayOfMonth.count() == 31) &&
    (dayOfWeek.count() == 7))
    dateFormat = (const char *)i18n("every day ").local8Bit();
  else
  {
    // Day of month not specified, so use day of week.
    if (dayOfMonth.count() == 31)
      dateFormat = "DOW_FORMAT";
    // Day of week not specified, so use day of month.
    if (dayOfWeek.count() == 7)
      dateFormat = "DOM_FORMAT";
  }

  // Replace tags to build natural language description.

  int pos(0);

  if ((pos = DOMFormat.find("DAYS_OF_MONTH")) > -1)
    DOMFormat.replace(pos,13,DOMDesc);

  if ((pos = DOMFormat.find("MONTHS")) > -1)
    DOMFormat.replace(pos,6,monthDesc);

  if ((pos = DOWFormat.find("DAYS_OF_WEEK")) > -1)
    DOWFormat.replace(pos,12,DOWDesc);

  if ((pos = dateFormat.find("DOM_FORMAT")) > -1)
    dateFormat.replace(pos,10,DOMFormat);

  if ((pos = dateFormat.find("DOW_FORMAT")) > -1)
    dateFormat.replace(pos,10,DOWFormat);

  if ((pos = timeFormat.find("TIME")) > -1)
    timeFormat.replace(pos,4,timeDesc);

  if ((pos = format.find("DATE_FORMAT")) > -1)
    format.replace(pos,11,dateFormat);

  if ((pos = format.find("TIME_FORMAT")) > -1)
    format.replace(pos,11,timeFormat);

  return format;
}

bool CTTask::system() const
{
  return syscron;
}
