
/***************************************************************************
 *   ktprintopt.h                                                          *
 *   --------------------------------------------------------------------  *
 *   Print Options Dialog                                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Robert Berry <rjmber@ntlwolrd.com>                *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#ifndef KTPRINTOPT_H
#define KTPRINTOPT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qdialog.h>

class QCheckBox;
class KAccel;

/**
  *Give the user the option to print the crontab file.
  *If the user is root ask if they want to print all the users
  */

class KTPrintOpt : public QDialog
{
  Q_OBJECT

public:

/**
  * Constructs the dialog, if root is true the print all users is not disabled
  */
  KTPrintOpt(bool root = false);

/**
  * Destructor
  */
  ~KTPrintOpt();

/**
  * Test if the root user wants to print all users
  */
  bool allUsers () {return printAllUsers;}

/**
  * Include the actual crontab file?
  */
  bool crontab  () {return printCrontab; }

private slots:
/**
  * Apply changes and close.
  */
  void slotOK();

/**
  * Cancel and close.
  */
  void slotCancel();

private:
  QCheckBox*      chkPrintCrontab;
  QCheckBox*      chkPrintAllUsers;

  QPushButton*    pbOk;
  QPushButton*    pbCancel;

  KAccel*         key_accel;

  bool            printCrontab;
  bool            printAllUsers;

};

#endif
