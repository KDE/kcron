/***************************************************************************
 *   resource.h                                                            *
 *   --------------------------------------------------------------------  *
 *   Macros used for commands                                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef RESOURCE_H
#define RESOURCE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// File-menu entries
#define ID_FILE_SAVE                10060
#define ID_FILE_PRINT               10080
#define ID_FILE_QUIT                10090

// Edit-menu entries
#define ID_EDIT_COPY                11010
#define ID_EDIT_CUT                 11020
#define ID_EDIT_PASTE               11030
#define ID_EDIT_NEW                 11040
#define ID_EDIT_MODIFY              11050
#define ID_EDIT_DELETE              11060
#define ID_EDIT_ENABLE              11070
#define ID_EDIT_RUNNOW              11080

// View-menu entries
#define ID_VIEW_TOOLBAR             12010
#define ID_VIEW_STATUSBAR           12020

// Help-menu entries
#define ID_HELP_CONTENTS            1002

// General application values
#define ID_STATUS_MSG               1001

#endif // RESOURCE_H

