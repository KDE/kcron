/***************************************************************************
 *   cthost.cpp                                                            *
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

#include "cthost.h"

#include "ctcron.h"

#include <unistd.h>  // getuid()

#include <sys/types.h>
#include <pwd.h>


using namespace std;

CTHost::CTHost()
{
  struct passwd *pwd = 0L;

  // If it is the root user
  if (getuid() == 0)
  {
    // Create the system cron table.
    createCTCron(true);

    // Read /etc/passwd
    setpwent(); // restart
    while((pwd=getpwent()))
    {
        createCTCron(pwd);
    }
    setpwent(); // restart again for others
  }
  else
  // Non-root user, so just create user's cron table.
  {
    createCTCron();
  }
}

CTHost::~CTHost()
{
  for (CTCronIterator i = cron.begin(); i != cron.end(); ++i)
    delete *i;
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
  CTCron *p = new CTCron(_syscron, _login);
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
  CTCron *p = new CTCron(pwd);
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
