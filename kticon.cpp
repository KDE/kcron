/***************************************************************************
 *   kticon.h                                                              *
 *   --------------------------------------------------------------------  *
 *   KT icons implementation.                                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "kticon.h"
 
#include <qimage.h>

#include <kglobal.h>
#include <kiconloader.h>

#include "ktuser.xpm"
#include "ktmuser.xpm"
#include "ktsystem.xpm"
#include "ktmsystem.xpm"
#include "ktvariable.xpm"
#include "ktmvariable.xpm"
#include "kthome.xpm"
#include "ktmhome.xpm"
#include "ktmail.xpm"
#include "ktmmail.xpm"
#include "ktpath.xpm"
#include "ktmpath.xpm"
#include "ktshell.xpm"
#include "ktmshell.xpm"
#include "kttask.xpm"
#include "ktmtask.xpm"

QPixmap KTIcon::getMiniIcon(const QString& name)
{
  return SmallIcon(name);
}

QPixmap KTIcon::getIcon(const QString& name)
{
  return BarIcon(name);
}

QPixmap KTIcon::application(bool mini)
{
  if (mini)
    return getMiniIcon("kcron");
  else
    return getIcon("kcron");
}

QPixmap KTIcon::system(bool mini)
{
  if (mini)
    return (const char**)ktmsystem;
  else
    return (const char**)ktsystem;
}

QPixmap KTIcon::user(bool mini)
{
  if (mini)
    return (const char**)ktmuser;
  else
    return (const char**)ktuser;
}

QPixmap KTIcon::variables(bool mini)
{
  if (mini)
    return getMiniIcon("folder");
  else
    return getIcon("folder");
}

QPixmap KTIcon::variable(bool mini)
{
  if (mini)
    return (const char**)ktmvariable;
  else
    return (const char**)ktvariable;
}

QPixmap KTIcon::mail(bool mini)
{
  if (mini)
    return (const char**)ktmmail;
  else
    return (const char**)ktmail;
}

QPixmap KTIcon::shell(bool mini)
{
  if (mini)
    return (const char**)ktmshell;
  else
    return (const char**)ktshell;
}

QPixmap KTIcon::home(bool mini)
{
  if (mini)
    return (const char**)ktmhome;
  else
    return (const char**)kthome;
}

QPixmap KTIcon::path(bool mini)
{
  if (mini)
    return (const char**)ktmpath;
  else
    return (const char**)ktpath;
}

QPixmap KTIcon::tasks(bool mini)
{
  if (mini)
    return getMiniIcon("folder");
  else
    return getIcon("folder");
}

QPixmap KTIcon::task(bool mini)
{
  if (mini)
    return (const char**)ktmtask;
  else
    return (const char**)kttask;
}
