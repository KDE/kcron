/***************************************************************************
 *   ktlistvar.h                                                           *
 *   --------------------------------------------------------------------  *
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <qlistview.h>
#include "ctcron.h"
#include "ctvariable.h"

/**
  * An enhanced QListViewItem that keeps a pointer to a CTVariable.
  */
class KTListVar : public QListViewItem
{
public:

/**
  * Initialize the list view item and environment variable.
  */
  KTListVar(QListViewItem* parent, CTCron* _ctcron,
    CTVariable* _ctvariable) :
    QListViewItem(parent),
    ctcron(_ctcron),
    ctvar(_ctvariable)
  {
    refresh();
    parent->setOpen(true);
  }

/**
  * Initialize from underlying variable.
  */
  void refresh()
  {
    // Is this breaking 2-byte support?
    setText (0,ctvar->variable.c_str());

    if (ctvar->enabled)
    {
      setText (1,ctvar->value.c_str());
      setText (2,ctvar->comment.c_str());
    }
    else
    {
      setText(1, "");
      setText(2, i18n("Disabled"));
    }

    if (ctvar->variable == "MAILTO")
      setPixmap(0, KTIcon::mail(true));
    else if (ctvar->variable == "SHELL")
      setPixmap(0, KTIcon::shell(true));
    else if (ctvar->variable == "HOME")
      setPixmap(0, KTIcon::home(true));
    else if (ctvar->variable == "PATH")
      setPixmap(0, KTIcon::path(true));
    else
      setPixmap(0, KTIcon::variable(true));

    parent()->setOpen(true);

  };

/**
  * Get the user's crontab.
  */
  CTCron* getCTCron() const
  {
    return ctcron;
  }

/**
  * Get the environment variable.
  */
  CTVariable* getCTVariable() const
  {
    return ctvar;
  }

private:

/**
  * User's crontab.
  */
  CTCron* ctcron;

/**
  * Environment variable.
  */
  CTVariable* ctvar;
};

#endif // KTLISTVAR_H




