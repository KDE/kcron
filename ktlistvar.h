/***************************************************************************
 *   KT list view item variable header.                                    *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTVAR_H
#define KTLISTVAR_H

#include <config.h>

#include "ktlistitem.h"

class CTVariable;

/**
  * QListViewItem with a CTVariable.
  */
class KTListVar : public KTListItem
{
public:

/**
  * Initialize the list view item and environment variable.
  */
  KTListVar(KTListItem* parent, CTCron* _ctcron, CTVariable* _ctvariable);

/**
  * Refresh from underlying variable.
  */
  void refresh();

/**
  * Print variable.
  */
   void print(KTPrint &printer) const;

/**
  * Edit variable.
  */
  virtual void edit();

/**
  * Get the environment variable.
  */
  CTVariable* getCTVariable() const;

private:

/**
  * Environment variable.
  */
  CTVariable* ctvar;
};

#endif // KTLISTVAR_H
