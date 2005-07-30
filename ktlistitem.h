/***************************************************************************
 *   KT list view item abstract base class.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTITEM_H
#define KTLISTITEM_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <q3listview.h>

class KTPrint;
class CTCron;

/**
  * An enhanced QListViewItem that ensures all list view items have the
  * same interface.
  */
class KTListItem : public Q3ListViewItem
{
public:

/**
  * Construct a root list view item from a CTCron.
  */
  KTListItem(KTListItem* parent, const char* name, CTCron* _ctcron);

/**
  * Construct a non-root list view item from a CTCron.
  */
  KTListItem(Q3ListView* parent, const char* name, CTCron* _ctcron);

/**
  * Destructor.
  */
  virtual ~KTListItem();

/**
  * Create.
  */
  virtual void create();

/**
  * Edit.
  */
  virtual void edit();

/**
  * Refresh.
  */
  virtual void refresh() = 0;

/**
  * Print.
  */
  virtual void print(KTPrint &printer) const =0;

/**
  * Get the user's crontab.
  */
  CTCron* getCTCron() const;

private:

/**
  * User's crontab.
  */
  CTCron* ctcron;

};

#endif // KTLISTITEM_H
