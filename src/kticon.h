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


/**
  * Wraps all icons used by the application.
  */

class KTIcon
{

public:

  enum IconSize {Small,Normal,Large};

  static QPixmap getMiniIcon(const QString& name);
  static QPixmap getIcon(const QString& name);
  static QPixmap getMaxIcon(const QString& name);
  static QPixmap application(int size);
  static QPixmap system(int size);
  static QPixmap user(int size);
  static QPixmap variables(int size);
  static QPixmap variable(int size);
  static QPixmap mail(int size);
  static QPixmap shell(int size);
  static QPixmap home(int size);
  static QPixmap path(int size);
  static QPixmap tasks(int size);
  static QPixmap task(int size);
  static QPixmap information(int size);

};

#endif // KTICON_H
