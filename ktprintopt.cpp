
/***************************************************************************
 *   ktprintopt.cpp                                                        *
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

#include <qcheckbox.h>
#include <qpushbutton.h>
#include <kaccel.h>
#include <klocale.h>
#include <kapp.h>
#include "ktprintopt.h"

KTPrintOpt::KTPrintOpt(bool root) :
    QDialog(0, (const char*)"ktprintopt", true, WStyle_DialogBorder), 
    printCrontab(false), printAllUsers(false)
{
  //printCrontab = false;
//  printAllUsers = false;

  chkPrintCrontab = new QCheckBox(i18n("Print Cron&tab"), this, "chkPrintCrontab");
  chkPrintCrontab->setGeometry(20, 10, 100, 25);

  chkPrintAllUsers = new QCheckBox(i18n("Print &All Users"), this, "chkPrintAllUsers");
  chkPrintAllUsers->setGeometry(20, 40, 100, 25);

  if (!root) {
    chkPrintAllUsers->setChecked(false);
    chkPrintAllUsers->setEnabled(false);
  }

  // OK
  pbOk = new QPushButton(i18n("&OK"), this, "pbOk");
  pbOk->setGeometry(40, 100, 80, 25);
  pbOk->setDefault(true);

  // Cancel
  pbCancel = new QPushButton(i18n("&Cancel"), this, "pbCancel");
  pbCancel->setGeometry(130, 100, 80, 25);

  connect(pbOk, SIGNAL(clicked()), SLOT(slotOK()));
  connect(pbCancel, SIGNAL(clicked()), SLOT(slotCancel()));

  // key acceleration
  key_accel = new KAccel(this);
  key_accel->connectItem(KStdAccel::Open, this, SLOT(slotOK()));
  key_accel->connectItem(KStdAccel::Close, this, SLOT(slotCancel()));
  key_accel->connectItem(KStdAccel::Quit, this, SLOT(slotCancel()));
  key_accel->readSettings();
}

KTPrintOpt::~KTPrintOpt()
{
// GARY!!
// I am assumming from the lack of destructor in KTTask.cpp that memory
// management is handled by the QDialog class.
}

void KTPrintOpt::slotOK()
{
  printCrontab = chkPrintCrontab->isChecked();
  printAllUsers = chkPrintAllUsers->isChecked();
  done(1);
}

void KTPrintOpt::slotCancel()
{
  close();
}
