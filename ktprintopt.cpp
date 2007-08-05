
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

#include <QCheckBox>
#include <QLayout>
//Added by qt3to4:
#include <QVBoxLayout>

#include <kdialog.h>

#include <klocale.h>
#include "ktprintopt.h"

KTPrintOpt::KTPrintOpt(bool root) : KPrintDialogPage()
{
  m_title = i18n("Cron Options");

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

void KTPrintOpt::setOptions(const QMap<QString,QString>& opts)
{
  QString  value;

  value = opts["crontab"];
  chkPrintCrontab->setChecked(value == "true");

  if (chkPrintAllUsers->isEnabled())
  {
     value = opts["allusers"];
     chkPrintAllUsers->setChecked(value == "true");
  }
}

void KTPrintOpt::getOptions(QMap<QString,QString>& opts, bool)
{
  opts["crontab"] = chkPrintCrontab->isChecked() ? "true" : "false";
  opts["allusers"] = chkPrintAllUsers->isChecked() ? "true" : "false";
}


#include "ktprintopt.moc"
