/***************************************************************************
 *   CT Cron Implementation                                                *
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

#include "ctcron.h"

#include "cti18n.h"
#include "cttask.h"
#include "ctvariable.h"
#include <unistd.h>      // getuid(), unlink()
#include <pwd.h>         // pwd, getpwnam(), getpwuid()
#include <stdio.h>
#include <assert.h>

#include <QFile>

#include <kshell.h>
#include <klocale.h>
#include <ktemporaryfile.h>

#include <iostream>

using namespace std;

CTCron::CTCron(const QString& crontabBinary, bool _syscron, string _login) :
  syscron(_syscron)
{
  int uid(getuid());

  this->crontab = crontabBinary;

  KTemporaryFile tmp;
  tmp.open();
  tmpFileName = tmp.fileName();  

  QString readCommand;

  if (uid == 0)
  // root, so provide requested crontab
  {
    if (syscron)
    {
      readCommand  = "cat /etc/crontab > " + KShell::quoteArg(tmpFileName);
      writeCommand = "cat " + KShell::quoteArg(tmpFileName) + " > /etc/crontab";
      login = (const char *)i18n("(System Crontab)").toLocal8Bit();
      name = "";
    }
    else
    {
      readCommand  = crontab + " -u " + _login.c_str() + " -l > " + KShell::quoteArg(tmpFileName);
      writeCommand = crontab + " -u " + _login.c_str() + ' ' + KShell::quoteArg(tmpFileName);
      if (!initFromPasswd(getpwnam(_login.c_str())))
      {
         error = i18n("No password entry found for user '%1'", _login.c_str());
      }
    }
  }
  else
  // regular user, so provide user's own crontab
  {
    readCommand  = crontab + " -l > " + KShell::quoteArg(tmpFileName);
    writeCommand = crontab + ' '      + KShell::quoteArg(tmpFileName);
    if (!initFromPasswd(getpwuid(uid)))
    {
      error = i18n("No password entry found for uid '%1'", uid);
    }
  }

  if (name.empty())
    name = login;

  initialTaskCount      = 0;
  initialVariableCount  = 0;
  
  if (isError())
     return;

  // Don't set error if it can't be read, it means the user
  // doesn't have a crontab.
  if (system(QFile::encodeName(readCommand)) == 0)
  {
    ifstream cronfile(QFile::encodeName(tmpFileName));
    cronfile >> *this;
  }

  initialTaskCount      = task.size();
  initialVariableCount  = variable.size();
}

CTCron::CTCron(const QString& crontabBinary, const struct passwd *pwd) :
  syscron(false)
{
  Q_ASSERT(pwd != 0L);

  crontab = crontabBinary;

  KTemporaryFile tmp;
  tmp.open();
  tmpFileName = tmp.fileName();  

  QString readCommand  = crontab + " -u " + QString(pwd->pw_name) + " -l > " + KShell::quoteArg(tmpFileName);
  writeCommand = crontab +  " -u " + QString(pwd->pw_name) + ' ' + KShell::quoteArg(tmpFileName);

  initFromPasswd(pwd);

  initialTaskCount      = 0;
  initialVariableCount  = 0;
  
  if (isError())
     return;

  // Don't set error if it can't be read, it means the user
  // doesn't have a crontab.
  if (system(QFile::encodeName(readCommand)) == 0)
  {
    ifstream cronfile(QFile::encodeName(tmpFileName));
    cronfile >> *this;
  }

  initialTaskCount      = task.size();
  initialVariableCount  = variable.size();
}

bool CTCron::initFromPasswd(const struct passwd *pwd)
{
   if (pwd == 0)
   {
      return false;
   }
   else
   {
      login = pwd->pw_name;
      name = pwd->pw_gecos;
      return true;
   }
}

void CTCron::operator = (const CTCron& source)
{
  assert(!source.syscron);

  for (CTVariableIterator i = const_cast<CTCron&>(source).variable.begin();
    i != source.variable.end(); ++i)
  {
    CTVariable* tmp = new CTVariable(**i);
    variable.push_back(tmp);
  }

  for (CTTaskIterator i = const_cast<CTCron&>(source).task.begin();
    i != source.task.end(); ++i)
  {
    CTTask* tmp = new CTTask(**i);
    task.push_back(tmp);
  }
}

istream& operator >> (istream& inputStream, CTCron& cron)
{
  const int MAX = 1024;
  char buffer[MAX];
  string line("");
  string comment("");

  while (inputStream)
  {
    inputStream.getline(buffer, MAX);
    line = buffer;

    // search for comments "#" but not disabled tasks "#\"
    if ((line.find("#") == 0) && (line.find("\\") != 1))
    {
      // If the first 10 characters don't contain a character, it's probably a disabled entry.
      int first_text = line.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
      if (first_text < 0)
         continue;

      if (first_text < 10) 
      {
         // remove leading pound sign
         line = line.substr(1,line.length()-1);
         // remove leading whitespace
         while (line.find_first_of(" \t") == 0)
            line = line.substr(1,line.length()-1);
         comment = line;
         continue;
      }
    }

    // else
    {
      // either a task or a variable
      int firstWhiteSpace(line.find_first_of(" \t"));
      int firstEquals(line.find("="));

      // if there is an equals sign and either there is no
      // whitespace or the first whitespace is after the equals
      // sign, it must be a variable
      if ((firstEquals > 0) && ((firstWhiteSpace == -1) ||
        firstWhiteSpace > firstEquals))
      {
        // create variable
        CTVariable *tmp = new CTVariable(line, comment);
        cron.variable.push_back(tmp);
        comment = "";
      }
      else
      // must be a task, either enabled or disabled
      {
        if (firstWhiteSpace > 0)
        {
          CTTask *tmp = new CTTask(line, comment, cron.syscron);
          cron.task.push_back(tmp);
          comment = "";
        }
      }
    }
  }
  return inputStream;
}

ostream& operator << (ostream& outputStream, const CTCron& cron)
{
  int itemCount(0);

  for (CTVariableIterator i = const_cast<CTCron&>(cron).variable.begin();
    i != cron.variable.end(); ++i)
  {
    outputStream << **i;
    itemCount++;
  }

  for (CTTaskIterator i = const_cast<CTCron&>(cron).task.begin();
    i != cron.task.end(); ++i)
  {
    outputStream << **i;
    itemCount++;
  }

  if (itemCount > 0)
  {
    outputStream << "# This file was written by KCron. Copyright (c) 1999, Gary Meyer\n";
    outputStream << "# Although KCron supports most crontab formats, use care when editing.\n";
    outputStream << "# Note: Lines beginning with \"#\\\" indicates a disabled task.\n";
  }

  return outputStream;
}

CTCron::~CTCron()
{
  for (CTTaskIterator i = task.begin(); i != task.end(); ++i)
    delete *i;
  for (CTVariableIterator i = variable.begin(); i != variable.end(); ++i)
    delete *i;
}

void CTCron::apply()
{
  // write to temp file
  ofstream cronfile(QFile::encodeName(tmpFileName));
  cronfile << *this << flush;

  // install temp file into crontab
  if (system(QFile::encodeName(writeCommand)) != 0)
  {
    error = i18n("An error occurred while updating crontab.");
  }

  // remove the temp file
  (void) unlink(QFile::encodeName(tmpFileName));

  if (isError())
    return;

  // mark as applied
  for (CTTaskIterator i = task.begin(); i != task.end(); ++i)
    (*i)->apply();

  for (CTVariableIterator i = variable.begin(); i != variable.end(); ++i)
    (*i)->apply();

  initialTaskCount = task.size();
  initialVariableCount = variable.size();
}

void CTCron::cancel()
{
  for (CTTaskIterator i = task.begin(); i != task.end(); ++i)
    (*i)->cancel();

  for (CTVariableIterator i = variable.begin(); i != variable.end(); ++i)
    (*i)->cancel();
}

bool CTCron::dirty()
{
  bool isDirty(false);

  if (initialTaskCount != task.size()) isDirty = true;

  if (initialVariableCount != variable.size()) isDirty = true;

  for (CTTaskIterator i = task.begin(); i != task.end(); ++i)
    if ((*i)->dirty()) isDirty = true;

  for (CTVariableIterator i = variable.begin(); i != variable.end(); ++i)
    if ((*i)->dirty()) isDirty = true;

  return isDirty;
}

string CTCron::path() const
{
  string path;

  for (CTVariableIterator var = const_cast<CTCron*>(this)->variable.begin();
    var != variable.end(); var++)
  {
    if ((*var)->variable == "PATH")
    {
      path = (*var)->value;
    }
  }
  return path;
}
