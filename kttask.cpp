/***************************************************************************
 *   kttask.cpp                                                            *
 *   --------------------------------------------------------------------  *
 *   KT task editor window implementation                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "kttask.h"

#include <qlabel.h>
#include <qstring.h>
#include <qfileinfo.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>

#include <kapplication.h>
#include <kaccel.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include "cttask.h"

#include "kticon.h"

KTTask::KTTask(CTTask* _cttask, const QString & _caption)
       :KDialog( 0, "kttask", true, WStyle_DialogBorder )
{
  cttask = _cttask;

  bool everyDay(true);

  QVBoxLayout *ml = new QVBoxLayout( this, KDialogBase::spacingHint() );

  QHBoxLayout *h1 = new QHBoxLayout( ml, KDialogBase::spacingHint() );

  // user
  labUser = new QLabel( i18n("&Run as:"), this, "labUser" );
  h1->addWidget( labUser );

  leUser = new QLineEdit( this, "leUser");
  labUser->setBuddy(leUser);
  h1->addWidget( leUser );

  if (cttask->system())
  {
    leUser->setText(QString::fromLocal8Bit(cttask->user.c_str()));
  }
  else
  {
    labUser->hide();
    leUser->hide();
  }

  // icon
  labIcon = new QLabel(this, "labIcon");
  labIcon->setFixedSize(32, 32);
  h1->addStretch( 1 );
  h1->addWidget( labIcon );

  // comment
  QHBoxLayout *h2 = new QHBoxLayout( ml, KDialogBase::spacingHint() );

  labComment = new QLabel( i18n("&Comment:"), this, "labComment" );
  h2->addWidget( labComment );

  leComment = new QLineEdit(this, "leComment");
  labComment->setBuddy(leComment);
  h2->addWidget( leComment );

  leComment->setText(QString::fromLocal8Bit(cttask->comment.c_str()));

  // command
  QHBoxLayout *h3 = new QHBoxLayout( ml, KDialogBase::spacingHint() );

  labCommand = new QLabel( i18n("&Program:"), this, "labCommand" );
  h3->addWidget( labCommand );

  leCommand = new QLineEdit(this, "leCommand");
  labCommand->setBuddy(leCommand);
  h3->addWidget( leCommand );

  leCommand->setText(QString::fromLocal8Bit(cttask->command.c_str()));

  labComment->setFixedWidth( QMAX( labComment->width(), labCommand->width()) );
  labCommand->setFixedWidth( QMAX( labComment->width(), labCommand->width()) );

  slotCommandChanged();

  pbBrowse = new QPushButton(this, "pbBrowse");
  pbBrowse->setText(i18n("&Browse..."));
  h3->addWidget( pbBrowse );

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), this, "chkEnabled");
  chkEnabled->setChecked(cttask->enabled);
  ml->addWidget( chkEnabled );

  // enabled
  chkSilent = new QCheckBox(i18n("&Silent"), this, "chkSilent");
  chkSilent->setChecked(cttask->silent);
  ml->addWidget( chkSilent );

  QHBoxLayout *h4 = new QHBoxLayout( ml, KDialogBase::spacingHint() );

  ml->addSpacing( 2 * KDialogBase::spacingHint() );

  // months
  bgMonth = new QButtonGroup( i18n("Months"), this, "bgMonth");
  h4->addWidget( bgMonth );

  QVBoxLayout *vmonths = new QVBoxLayout( bgMonth, KDialogBase::spacingHint() );
  vmonths->addSpacing( 2 * KDialogBase::spacingHint() );

  for (int mo = 1; mo <= 12; mo++)
  {
    cbMonth[mo] = new QCheckBox(bgMonth, "cbMonth");
    cbMonth[mo]->setText(QString::fromLocal8Bit(cttask->month.getName(mo).c_str()));
    cbMonth[mo]->setChecked(cttask->month.get(mo));
    vmonths->addWidget( cbMonth[mo], AlignLeft );

    if (!cttask->month.get(mo)) everyDay = false;
  }

  QVBoxLayout *v1 = new QVBoxLayout( h4, KDialogBase::spacingHint() );

  // days of the month
  bgDayOfMonth = new QButtonGroup( i18n("Days of the Month"), this, "bgDayOfMonth");
  v1->addWidget( bgDayOfMonth );

  QPushButton* day;
  QString tmp;

  QVBoxLayout *vdays = new QVBoxLayout( bgDayOfMonth, KDialogBase::spacingHint() );
  vdays->addSpacing( 2 * KDialogBase::spacingHint() );
  QHBoxLayout *hdays = 0;

  for (int dm = 1; dm <= 31; dm++)
  {
    if( (dm % 7) == 1 )
      hdays = new QHBoxLayout( vdays, KDialogBase::spacingHint() );

    day = new QPushButton(bgDayOfMonth);
    day->setFixedSize(25, 25);
    day->setText(tmp.setNum(dm));
    day->setToggleButton(true);
    day->setOn(cttask->dayOfMonth.get(dm));
    pbDayOfMonth[dm] = day;
    if (!cttask->dayOfMonth.get(dm)) everyDay = false;

    hdays->addWidget( day, AlignLeft );
  }
  hdays->addStretch( 1 );

  // days of the week
  bgDayOfWeek = new QButtonGroup( i18n("Days of the Week"), this, "bgDayOfWeek");
  v1->addWidget( bgDayOfWeek );

  QVBoxLayout *v3 = new QVBoxLayout( bgDayOfWeek, KDialogBase::spacingHint() );
  v3->addSpacing( 2 * KDialogBase::spacingHint() );

  for (int dw = 1; dw <= 7; dw++)
  {
    cbDayOfWeek[dw] = new QCheckBox(bgDayOfWeek);
    cbDayOfWeek[dw]->setText(QString::fromLocal8Bit(cttask->dayOfWeek.getName(dw).c_str()));
    cbDayOfWeek[dw]->setChecked(cttask->dayOfWeek.get(dw));
    v3->addWidget( cbDayOfWeek[dw] );

    if (!cttask->dayOfWeek.get(dw)) everyDay = false;
  }

  QVBoxLayout *v2 = new QVBoxLayout( h4, KDialogBase::spacingHint() );

  // daily
  bgEveryDay = new QButtonGroup( i18n("Daily"), this, "bgEveryDay");
  v2->addWidget( bgEveryDay );

  QVBoxLayout *v9 = new QVBoxLayout( bgEveryDay, KDialogBase::spacingHint() );
  v9->addSpacing( 2 * KDialogBase::spacingHint() );

  cbEveryDay = new QCheckBox( i18n("Run every day"), bgEveryDay, "cbEveryDay");
  cbEveryDay->setChecked(everyDay);
  v9->addWidget( cbEveryDay );

  // hours
  bgHour = new QButtonGroup( i18n("Hours"), this, "bgHour");
  v2->addWidget( bgHour );
  QVBoxLayout *v4 = new QVBoxLayout( bgHour, KDialogBase::spacingHint() );
  v4->addSpacing( 2 * KDialogBase::spacingHint() );

  labAM = new QLabel( i18n("AM"), bgHour, "labAM");
  labAM->setAlignment(AlignRight | AlignVCenter);
  v4->addWidget( labAM );


  for (int ho = 0; ho <= 23; ho++)
  {
    pbHour[ho] = new QPushButton(bgHour);
    pbHour[ho]->setText(tmp.setNum(ho));
    pbHour[ho]->setToggleButton(true);
    pbHour[ho]->setOn(cttask->hour.get(ho));
    pbHour[ho]->setFixedSize(25, 25);
  }

  QHBoxLayout *hhours = new QHBoxLayout( v4, KDialogBase::spacingHint() );
  for (int ho1 = 0; ho1 <= 11; ho1++)
  {
    if( ho1 == 6 )
      hhours = new QHBoxLayout( v4, KDialogBase::spacingHint() );

    hhours->addWidget( pbHour[ho1] );
  }

  labPM = new QLabel( i18n("PM"), bgHour, "labPM");
  labPM->setAlignment(AlignRight | AlignVCenter);
  v4->addWidget( labPM );

  hhours = new QHBoxLayout( v4, KDialogBase::spacingHint() );
  for (int ho1 = 12; ho1 <= 23; ho1++)
  {
    if( ho1 == 18 )
      hhours = new QHBoxLayout( v4, KDialogBase::spacingHint() );

    hhours->addWidget( pbHour[ho1] );
  }
  // minutes
  bgMinute = new QButtonGroup( i18n("Minutes"), this, "bgMinute");
  v2->addWidget( bgMinute );
  QVBoxLayout *vmin = new QVBoxLayout( bgMinute, KDialogBase::spacingHint() );
  vmin->addSpacing( 2 * KDialogBase::spacingHint() );

  for (int mi = 0; mi <= 55; mi+=5)
  {
    pbMinute[mi] = new QPushButton(bgMinute);
    pbMinute[mi]->setText(tmp.setNum(mi));
    pbMinute[mi]->setToggleButton(true);
    pbMinute[mi]->setOn(cttask->minute.get(mi));
    pbMinute[mi]->setFixedSize(25, 25);
  }

  QHBoxLayout *hmin = new QHBoxLayout( vmin, KDialogBase::spacingHint() );
  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    if( mi1 == 30 )
      hmin = new QHBoxLayout( vmin, KDialogBase::spacingHint() );

    hmin->addWidget( pbMinute[mi1] );
  }

  QHBoxLayout *h5 = new QHBoxLayout( ml, KDialogBase::spacingHint() );
  h5->addStretch( 1 );

  // OK
  pbOk = new QPushButton(i18n("&OK"), this, "pbOk");
  pbOk->setDefault(true);
  h5->addWidget( pbOk );

  // Cancel
  pbCancel = new QPushButton(i18n("&Cancel"), this, "pbCancel");
  h5->addWidget( pbCancel );

  // window
  setIcon(KTIcon::application(true));
  setCaption(_caption/*i18n("Edit Task")*/);

  // set focus to first widget
  if (cttask->system())
  {
    leUser->setFocus();
  }
  else
  {
    leComment->setFocus();
  }

  // connect them up
  connect(pbBrowse, SIGNAL(clicked()), SLOT(slotBrowse()));
  connect(leCommand, SIGNAL(textChanged(const QString&)),
    SLOT(slotCommandChanged()));
  connect(cbEveryDay, SIGNAL(clicked()), SLOT(slotDailyChanged()));
  connect(pbOk, SIGNAL(clicked()), SLOT(slotOK()));
  connect(pbCancel, SIGNAL(clicked()), SLOT(slotCancel()));

  // key acceleration
  key_accel = new KAccel(this);
  key_accel->connectItem(KStdAccel::Open, this, SLOT(slotOK()));
  key_accel->connectItem(KStdAccel::Close, this, SLOT(slotCancel()));
  key_accel->connectItem(KStdAccel::Quit, this, SLOT(slotCancel()));
  key_accel->readSettings();

  setFixedSize( minimumSize() );
  slotDailyChanged();
}

KTTask::~KTTask()
{
}

void KTTask::slotCommandChanged()
{
  /*
  QString qs(leCommand->text());

  int beginPos(qs.findRev("/", qs.length()) + 1);
  if (beginPos < 0) beginPos = 0;

  int endPos(qs.findRev(" ", qs.length()));
  if (endPos < 0) endPos = qs.length();

  QString iconName(qs.mid(beginPos, endPos-beginPos) + ".xpm");

  QPixmap qp(KTIcon::getIcon(iconName));
  if (qp.isNull())
    labIcon->setPixmap(KTIcon::task(false));
  else
    labIcon->setPixmap(qp);
  */

  labIcon->setPixmap(KTIcon::task(false));
  return;
}

void KTTask::slotDailyChanged()
{
  if (cbEveryDay->isChecked())
  {
    for (int mo = 1; mo <= 12; mo++)
    {
      cbMonth[mo]->setChecked(true);
      cbMonth[mo]->setEnabled(false);
    }
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setOn(true);
      pbDayOfMonth[dm]->setEnabled(false);
    }
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(true);
      cbDayOfWeek[dw]->setEnabled(false);
    }
  }
  else
  {
    for (int mo = 1; mo <= 12; mo++)
    {
      cbMonth[mo]->setEnabled(true);
    }
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setEnabled(true);
    }
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setEnabled(true);
    }
  }
}

void KTTask::slotOK()
{
  // Make it friendly for just selecting days of the month or
  // days of the week.

  int monthDaysSelected(0);
  for (int dm = 1; dm <= 31; dm++)
  {
    if (pbDayOfMonth[dm]->isOn()) monthDaysSelected++;
  }

  int weekDaysSelected(0);
  for (int dw = 1; dw <= 7; dw++)
  {
    if (cbDayOfWeek[dw]->isChecked()) weekDaysSelected++;
  }

  if ((monthDaysSelected == 0) && (weekDaysSelected > 0))
  {
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setOn(1);
    }
  }

  if ((weekDaysSelected == 0) && (monthDaysSelected > 0))
  {
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(1);
    }
  }

  // Now validate
  QString message(i18n("Please enter the following to schedule the task:\n"));
  QString sep("\n- ");
  bool showMessage(false);

  if (QString(leCommand->text()) == "")
  {
    message += sep + i18n("the program to run");
    leCommand->setFocus();
    showMessage = true;
  }

  bool valid(false);
  for (int mo = 1; mo <= 12; mo++)
  {
    if (cbMonth[mo]->isChecked()) valid = true;
  }
  if (!valid)
  {
    message += sep + i18n("the months");
    if (!showMessage)
    {
      cbMonth[1]->setFocus();
    }
    showMessage = true;
  }

  valid = false;
  for (int dm = 1; dm <= 31; dm++)
  {
    if (pbDayOfMonth[dm]->isOn()) valid = true;
  }
  for (int dw = 1; dw <= 7; dw++)
  {
    if (cbDayOfWeek[dw]->isChecked()) valid = true;
  }

  if (!valid)
  {
    message += sep +
      i18n("either the days of the month or the days of the week");
    if (!showMessage)
    {
      pbDayOfMonth[1]->setFocus();
    }
    showMessage = true;
  }

  valid = false;
  for (int ho = 0; ho <= 23; ho++)
  {
    if (pbHour[ho]->isOn()) valid = true;
  }

  if (!valid)
  {
    message += sep + i18n("the hours");
    if (!showMessage)
    {
      pbHour[0]->setFocus();
    }
    showMessage = true;
  }

  valid = false;
  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    if (pbMinute[mi1]->isOn()) valid = true;
  }

  if (!valid)
  {
    message += sep + i18n("the minutes");
    if (!showMessage)
    {
      pbMinute[0]->setFocus();
    }
    showMessage = true;
  }

  if (showMessage)
  {
    KMessageBox::information(this, message);
    return;
  }

  // make sure the file name is a good one if we have an
  // absolute path

  QString qs(leCommand->text());
  if (qs.find("/") == 0)
  {
    int spacePos(qs.find(" "));
    if (spacePos < 0) spacePos = qs.length();
    QString programName(qs.left(spacePos));
    QFileInfo file(programName);

    if (!file.isReadable())
    {
      KMessageBox::information(this,
        i18n("Cannot locate program. Please re-enter."));
      leCommand->setFocus();
      return;
    }

    if (!file.isExecutable())
    {
      KMessageBox::information(this,
        i18n("Program is not an executable file. Please re-enter."));
      leCommand->setFocus();
      return;
    }
  }

  // save work in process
  if (cttask->user != "")
  {
    cttask->user = QFile::encodeName(leUser->text());
  }
  else
  {
    cttask->user = "";
  }

  cttask->comment = (const char *)leComment->text().local8Bit();
  cttask->command = (const char *)leCommand->text().local8Bit();
  cttask->enabled = chkEnabled->isChecked();
  cttask->silent = chkSilent->isChecked();

  for (int mo = 1; mo <= 12; mo++)
  {
    cttask->month.set(mo, cbMonth[mo]->isChecked());
  }

  for (int dm = 1; dm <= 31; dm++)
  {
    cttask->dayOfMonth.set(dm, pbDayOfMonth[dm]->isOn());
  }
  for (int dw = 1; dw <= 7; dw++)
  {
    cttask->dayOfWeek.set(dw, cbDayOfWeek[dw]->isChecked());
  }
  for (int ho = 0; ho <= 23; ho++)
  {
    cttask->hour.set(ho, pbHour[ho]->isOn());
  }
  for (int mi = 0; mi <= 59; mi++)
  {
    cttask->minute.set(mi, false);
  }
  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    cttask->minute.set(mi1, pbMinute[mi1]->isOn());
  }

  close();
}

void KTTask::slotCancel()
{
  close();
}

void KTTask::slotBrowse()
{
  KURL url = KFileDialog::getOpenURL();

  if(!url.isEmpty())
  {
    if(url.isLocalFile())
    {
      leCommand->setText(url.path());
    }
    else
    {
      KMessageBox::sorry(this,
        i18n("Only local or mounted files can be executed by crontab."));
    }
  }

  leCommand->setFocus();
}

#include "kttask.moc"
