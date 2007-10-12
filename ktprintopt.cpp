
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

#include "ktprintopt.h"

#include <QCheckBox>
#include <QLayout>
//Added by qt3to4:
#include <QVBoxLayout>

#include <kdialog.h>
#include <klocale.h>

KTPrintOpt::KTPrintOpt(bool root) : QWidget()
{
  setWindowTitle(i18n("Cron Options"));

  QVBoxLayout *main_ = new QVBoxLayout(this);
  main_->setMargin(KDialog::marginHint());
  main_->setSpacing(KDialog::spacingHint());

  chkPrintCrontab = new QCheckBox(i18n("Print cron&tab"), this);
  chkPrintCrontab->setObjectName("chkPrintCrontab");
  main_->addWidget(chkPrintCrontab);

  chkPrintAllUsers = new QCheckBox(i18n("Print &all users"), this);
  chkPrintAllUsers->setObjectName("chkPrintAllUsers");
  main_->addWidget(chkPrintAllUsers);

  if (!root) {
    chkPrintAllUsers->setChecked(false);
    chkPrintAllUsers->setEnabled(false);
  }

  setLayout(main_);
}

KTPrintOpt::~KTPrintOpt()
{
}

bool KTPrintOpt::printCrontab()
{
  return chkPrintCrontab->isChecked();
}

void KTPrintOpt::setPrintCrontab( bool setStatus )
{
  chkPrintCrontab->setChecked(setStatus);
}

bool KTPrintOpt::printAllUsers()
{
  return chkPrintAllUsers->isChecked();
}

void KTPrintOpt::setPrintAllUsers( bool setStatus )
{
  chkPrintAllUsers->setChecked(setStatus);
}

#include "ktprintopt.moc"
