/***************************************************************************
 *   cti18n.h                                                              *
 *   --------------------------------------------------------------------  *
 *   CT Internationalization Header                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTI18N_H
#define CTI18N_H

#ifdef KDE
#ifdef KDE1
#include <kapp.h>
#include <klocale.h>
#define cti18n(X) i18n(X)
#endif
#endif

#ifdef KDE
#ifndef KDE1
#include <klocale.h>
#define cti18n(X) (const char*)i18n(X)
#endif
#endif

#ifndef cti18n
#define cti18n(X) X
#endif

#endif // CTI18N_H
