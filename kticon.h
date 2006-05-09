/***************************************************************************
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
 
#include <qpixmap.h>
#include <QString>

/**
  * Wraps all icons used by the application.
  */

class KTIcon
{

public:

  static QPixmap getMiniIcon(const QString& name);
  static QPixmap getIcon(const QString& name);
  static QPixmap application(bool mini);
  static QPixmap system(bool mini);
  static QPixmap user(bool mini);
  static QPixmap variables(bool mini);
  static QPixmap variable(bool mini);
  static QPixmap mail(bool mini);
  static QPixmap shell(bool mini);
  static QPixmap home(bool mini);
  static QPixmap path(bool mini);
  static QPixmap tasks(bool mini);
  static QPixmap task(bool mini);

};

#endif // KTICON_H
