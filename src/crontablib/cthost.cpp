/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   CT Host Implementation                                                *
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
// aacid: Sorry we did, anyway you don't seem to be mantaining kcron anymore - 05/07

#include "cthost.h"

#include "ctcron.h"
#include "cti18n.h"

#include <unistd.h>  // getuid()

#include <sys/types.h>
#include <pwd.h>

#include <qfile.h>

#include <QTextStream>

using namespace std;

CTHost::CTHost(const QString& cronBinary)
{
  struct passwd *pwd = 0L;

  this->crontabBinary = cronBinary;

  // If it is the root user
  if (getuid() == 0)
  {
    // Create the system cron table.
    createCTCron(true);

    // Read /etc/passwd
    setpwent(); // restart
    while((pwd=getpwent()))
    {
      if (allowDeny(pwd->pw_name))
        createCTCron(pwd);
    }
    setpwent(); // restart again for others
  }
  else
  // Non-root user, so just create user's cron table.
  {
    // Get name from UID, check it against AllowDeny()
    unsigned int uid = getuid();
    setpwent(); // restart
    while((pwd=getpwent())) 
    {
      if ((pwd->pw_uid == uid) && (!allowDeny(pwd->pw_name)))
      {
        error = i18n("You have been blocked from using KCron\
                      by either the /etc/cron.allow file or the /etc/cron.deny file.\
                      \n\nCheck the crontab man page for further details.");
        return;
      }
    }
    setpwent(); // restart again for others
    createCTCron();
  }
}

CTHost::~CTHost()
{
  for (CTCronIterator i = cron.begin(); i != cron.end(); ++i)
    delete *i;
}

bool CTHost::allowDeny(char *name)
{
  QFile allow("/etc/cron.allow");

  if (allow.open(IO_ReadOnly))  // if cron.allow exists make sure user is listed
  {
    QTextStream stream(&allow);
    while (!stream.atEnd())
    {
      if (stream.readLine() == name)
      {
        allow.close();
        return true;
      }
    }
    allow.close();
    return false;
  }
  else
  {
    allow.close();
    QFile deny("/etc/cron.deny");
    if (deny.open(IO_ReadOnly)) // else if cron.deny exists make sure user is not listed
    {
      QTextStream stream(&deny);
      while (!stream.atEnd())
      {
        if (stream.readLine() == name)
        {
          deny.close();
          return false;
        }
      }
      deny.close();
      return true;
    }
    else
    {
      deny.close();
      return true;
    }
  }
}

void CTHost::apply()
{
  for (CTCronIterator i = cron.begin(); i != cron.end(); ++i)
  {
    (*i)->apply();
    if ((*i)->isError())
    {
       error = (*i)->errorMessage();
       return;
    }
  }
}

void CTHost::cancel()
{
  for (CTCronIterator i = cron.begin(); i != cron.end(); ++i)
    (*i)->cancel();
}

bool CTHost::dirty()
{
  bool isDirty(false);

  for (CTCronIterator i = cron.begin(); i != cron.end(); ++i)
    if ((*i)->dirty()) isDirty = true;

  return isDirty;
}

CTCron* CTHost::createCTCron(bool _syscron, string _login)
{
  CTCron *p = new CTCron(crontabBinary, _syscron, _login);
  if (p->isError())
  {
     error = p->errorMessage();
     delete p;
     return 0;
  }
  cron.push_back(p);
  return p;
}

CTCron* CTHost::createCTCron(const struct passwd *pwd)
{
  CTCron *p = new CTCron(crontabBinary, pwd);
  if (p->isError())
  {
    error = p->errorMessage();
    delete p;
    return 0;
  }
  cron.push_back(p);
  return p;
}

bool CTHost::root() const
{
  return (!getuid());
}
