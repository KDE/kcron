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

// Do not introduce any Qt or KDE dependencies into the "CT"-prefixed classes.
// I want to be able to reuse these classes with another GUI toolkit. -GM 11/99
//
// Note: I do make an exception to the above rule for this class, necessary
// to support internationalization.  It is controlled with macros, but still
// don't introduce any other Qt or KDE dependencies here. -GM 11/99

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
