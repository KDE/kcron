
/***************************************************************************
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

#include <QWidget>

class QCheckBox;

/**
  *Give the user the option to print the crontab file.
  *If the user is root ask if they want to print all the users
  */

class KTPrintOpt : public QWidget
{
  Q_OBJECT
public:

/**
  * Constructs the dialog, if root is true the print all users is not disabled
  */
  KTPrintOpt(bool root = false);

  ~KTPrintOpt();

  bool printCrontab();
  void setPrintCrontab( bool setStatus );

  bool printAllUsers();
  void setPrintAllUsers( bool setStatus );

private:
  QCheckBox*      chkPrintCrontab;
  QCheckBox*      chkPrintAllUsers;
};

#endif
