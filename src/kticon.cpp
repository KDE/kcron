/***************************************************************************
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
//Added by qt3to4:
#include <QPixmap>

#include <kglobal.h>
#include <kiconloader.h>



QPixmap KTIcon::getMiniIcon(const QString& name)
{
  return SmallIcon(name);
}

QPixmap KTIcon::getIcon(const QString& name)
{
  return BarIcon(name);
}

QPixmap KTIcon::getMaxIcon(const QString& name)
{
  return DesktopIcon(name);
}

QPixmap KTIcon::application(int size)
{
  if (size == 0) return getMiniIcon("kcron");
  if (size == 1) return getIcon("kcron");
  return getMaxIcon("kcron");
}

QPixmap KTIcon::system(int size)
{
  if (size == 0) return getMiniIcon("utilities-terminal");
  if (size == 1) return getIcon("utilities-terminal");
  return getMaxIcon("utilities-terminal");
}

QPixmap KTIcon::user(int size)
{
  if (size == 0) return getMiniIcon("user-identity");
  if (size == 1) return getIcon("user-identity");
  return getMaxIcon("user-identity");
}

QPixmap KTIcon::variables(int size)
{
  if (size == 0) return getMiniIcon("folder");
  if (size == 1) return getIcon("folder");
  return getMaxIcon("folder");
}

QPixmap KTIcon::variable(int size)
{
  if (size == 0) return getMiniIcon("application-x-zerosize");
  if (size == 1) return getIcon("application-x-zerosize");
  return getMaxIcon("application-x-zerosize");
}

QPixmap KTIcon::mail(int size)
{
  if (size == 0) return getMiniIcon("mail-message");
  if (size == 1) return getIcon("mail-message");
  return getMaxIcon("mail-message");
}

QPixmap KTIcon::shell(int size)
{
  if (size == 0) return getMiniIcon("utilities-terminal");
  if (size == 1) return getIcon("utilities-terminal");
  return getMaxIcon("utilities-terminal");
}

QPixmap KTIcon::home(int size)
{
  if (size == 0) return getMiniIcon("go-home");
  if (size == 1) return getIcon("go-home");
  return getMaxIcon("go-home");
}

QPixmap KTIcon::path(int size)
{
  if (size == 0) return getMiniIcon("folder");
  if (size == 1) return getIcon("folder");
  return getMaxIcon("folder");
}

QPixmap KTIcon::tasks(int size)
{
  if (size == 0) return getMiniIcon("folder");
  if (size == 1) return getIcon("folder");
  return getMaxIcon("folder");
}

QPixmap KTIcon::task(int size)
{
  if (size == 0) return getMiniIcon("system-run");
  if (size == 1) return getIcon("system-run");
  return getMaxIcon("system-run");
}

QPixmap KTIcon::information(int size)
{
  if (size == 0) return getMiniIcon("dialog-information");
  if (size == 1) return getIcon("dialog-information");
  return getMaxIcon("dialog-information");
}
