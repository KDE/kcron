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
// Note: I do make an exception to the above rule for this file, as it is
// necessary to support internationalization.
//
// Regardless, please don't introduce any other Qt or KDE dependencies here.
// -GM 11/99

#include <klocale.h>
#include <kapp.h>

#endif // CTI18N_H
