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
 
#ifdef KDE1
#include <kapp.h>
#else
#include <kglobal.h>
#endif

#include <qimage.h>
#include <kiconloader.h>

#include "ktapp.xpm"
#include "ktmapp.xpm"
#include "ktuser.xpm"
#include "ktmuser.xpm"
#include "ktsystem.xpm"
#include "ktmsystem.xpm"
#include "ktfolder.xpm"
#include "ktmfolder.xpm"
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
#ifdef KDE1
  return KApplication::getKApplication()->getIconLoader()->loadMiniIcon(name);
#else
  return SmallIcon(name);
#endif
}

QPixmap KTIcon::getIcon(const QString& name)
{
#ifdef KDE1
  return KApplication::getKApplication()->getIconLoader()->loadIcon(name);
#else
  return BarIcon(name);
#endif
}

QPixmap KTIcon::save()
{
  return getIcon("filefloppy.xpm");
}

QPixmap KTIcon::print()
{
  return getIcon("fileprint.xpm");
}

QPixmap KTIcon::cut()
{
  return getIcon("editcut.xpm");
}

QPixmap KTIcon::copy()
{
  return getIcon("editcopy.xpm");
}

QPixmap KTIcon::paste()
{
  return getIcon("editpaste.xpm");
}

QPixmap KTIcon::help()
{
  return getIcon("help.xpm");
}

QPixmap KTIcon::application(bool mini)
{
  if (mini)
    return (const char**)ktmapp;
  else
    return (const char**)ktapp;
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
    return (const char**)ktmfolder;
  else
    return (const char**)ktfolder;
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
    return (const char**)ktmfolder;
  else
    return (const char**)ktfolder;
}

QPixmap KTIcon::task(bool mini)
{
  if (mini)
    return (const char**)ktmtask;
  else
    return (const char**)kttask;
}
