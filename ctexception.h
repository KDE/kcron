/***************************************************************************
 *   ctexception.h                                                         *
 *   --------------------------------------------------------------------  *
 *   CT Exception Header                                                   *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTEXCEPTION_H
#define CTEXCEPTION_H

/**
  * Base exception.
  */
class CTException {};

/**
  * Stream, such as file, i/o exception.
  */
class CTExceptionIO : public CTException {};

#endif // CTEXCEPTION_H
