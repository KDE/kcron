/***************************************************************************
 *   kticon.h                                                              *
 *   --------------------------------------------------------------------  *
 *   KT icons.                                                             *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTICON_H
#define KTICON_H
 
#include <iostream.h>

#include <qimage.h>
#ifdef KDE1
#include <kapp.h>
#else
#include <kglobal.h>
#endif
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

/**
  * Wraps all icons used by the application.
  */

class KTIcon
{

public:

  static QPixmap getMiniIcon(const QString& name)
  {
#ifdef KDE1
    return KApplication::getKApplication()->getIconLoader()->loadMiniIcon(name);
#else
    return KGlobal::iconLoader()->loadApplicationMiniIcon(name);
#endif
  }

  static QPixmap getIcon(const QString& name)
  {
#ifdef KDE1
    return KApplication::getKApplication()->getIconLoader()->loadIcon(name);
#else
    return KGlobal::iconLoader()->loadIcon(name);
#endif
  }

  static QPixmap save()
    { return getIcon("filefloppy.xpm"); }
  static QPixmap print()
    { return getIcon("fileprint.xpm"); }
  static QPixmap cut()
    { return getIcon("editcut.xpm"); }
  static QPixmap copy()
    { return getIcon("editcopy.xpm"); }
  static QPixmap paste()
    { return getIcon("editpaste.xpm"); }
  static QPixmap help()
    { return getIcon("help.xpm"); }

  static QPixmap application(bool mini)
  {
    if (mini)
      return (const char**)ktmapp;
    else
      return (const char**)ktapp;
  }
  static QPixmap system(bool mini)
  {
    if (mini)
      return (const char**)ktmsystem;
    else
      return (const char**)ktsystem;
  }
  static QPixmap user(bool mini)
  {
    if (mini)
      return (const char**)ktmuser;
    else
      return (const char**)ktuser;
  }
  static QPixmap variables(bool mini)
  {
    if (mini)
      return (const char**)ktmfolder;
    else
      return (const char**)ktfolder;
  }
  static QPixmap variable(bool mini)
  {
    if (mini)
      return (const char**)ktmvariable;
    else
      return (const char**)ktvariable;
  }
  static QPixmap mail(bool mini)
  {
    if (mini)
      return (const char**)ktmmail;
    else
      return (const char**)ktmail;
  }
  static QPixmap shell(bool mini)
  {
    if (mini)
      return (const char**)ktmshell;
    else
      return (const char**)ktshell;
  }
  static QPixmap home(bool mini)
  {
    if (mini)
      return (const char**)ktmhome;
    else
      return (const char**)kthome;
  }
  static QPixmap path(bool mini)
  {
    if (mini)
      return (const char**)ktmpath;
    else
      return (const char**)ktpath;
  }
  static QPixmap tasks(bool mini)
  {
    if (mini)
      return (const char**)ktmfolder;
    else
      return (const char**)ktfolder;
  }
  static QPixmap task(bool mini)
  {
    if (mini)
      return (const char**)ktmtask;
    else
      return (const char**)kttask;
  }
};

#endif // KTICON_H

