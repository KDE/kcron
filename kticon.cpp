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
    return getMiniIcon("openterm");
  else
    return getIcon("openterm");
}

QPixmap KTIcon::user(bool mini)
{
  if (mini)
    return getMiniIcon("kuser");
  else
    return getIcon("kuser");
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
    return getMiniIcon("mime_empty");
  else
    return getIcon("mime_empty");
}

QPixmap KTIcon::mail(bool mini)
{
  if (mini)
    return getMiniIcon("mail_generic");
  else
    return getIcon("mail_generic");
}

QPixmap KTIcon::shell(bool mini)
{
  if (mini)
    return getMiniIcon("openterm");
  else
    return getIcon("openterm");
}

QPixmap KTIcon::home(bool mini)
{
  if (mini)
    return getMiniIcon("gohome");
  else
    return getIcon("gohome");
}

QPixmap KTIcon::path(bool mini)
{
  if (mini)
    return getMiniIcon("folder");
  else
    return getIcon("folder");
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
    return getMiniIcon("gear");
  else
    return getIcon("gear");
}
