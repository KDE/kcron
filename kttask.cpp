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
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qmessagebox.h>    // These should be replaced by KMessageBox
#include <qbuttongroup.h>

#include <kapp.h>
#include <kaccel.h>
#include <klocale.h>
#include <kfiledialog.h>
#include <kmessagebox.h>

#include "cttask.h"

#include "kticon.h"

KTTask::KTTask(CTTask* _cttask) :
  QDialog(0, (const char*)"kttask", true, WStyle_DialogBorder)
{
  cttask = _cttask;

  bool everyDay(true);

  // user
  labUser = new QLabel(this, "labUser");
  labUser->setGeometry(10, 17, 70, 25);
  labUser->setText(i18n("&Run as:"));

  leUser = new QLineEdit(this, "leUser");
  leUser->setGeometry(90, 17, 120, 25);
  labUser->setBuddy(leUser);

  if (cttask->system())
  {
    leUser->setText(cttask->user.c_str());
  }
  else
  {
    labUser->hide();
    leUser->hide();
  }

  // icon
  labIcon = new QLabel(this, "labIcon");
  labIcon->setGeometry(418, 10, 32, 32);

  // comment
  labComment = new QLabel(this, "labComment");
  labComment->setGeometry(10, 52, 70, 25);
  labComment->setText(i18n("&Comment:"));

  leComment = new QLineEdit(this, "leComment");
  leComment->setGeometry(90, 52, 360, 25);
  leComment->setMaxLength(100);
  labComment->setBuddy(leComment);

  leComment->setText(cttask->comment.c_str());

  // command
  labCommand = new QLabel(this, "labCommand");
  labCommand->setGeometry(10, 87, 70, 25);
  labCommand->setText(i18n("&Program:"));

  leCommand = new QLineEdit(this, "leCommand");
  leCommand->setGeometry(90, 87, 280, 25);
  // leCommand->setGeometry(90, 87, 360, 25);
  leCommand->setMaxLength(100);
  labCommand->setBuddy(leCommand);

  leCommand->setText(cttask->command.c_str());

  slotCommandChanged();

  pbBrowse = new QPushButton(this, "pbBrowse");
  pbBrowse->setGeometry(380, 87, 70, 25);
  pbBrowse->setText(i18n("&Browse..."));

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), this, "chkEnabled");
  chkEnabled->setGeometry(10, 122, 350, 25);

  chkEnabled->setChecked(cttask->enabled);

  // months
  bgMonth = new QButtonGroup(this, "bgMonth");
  bgMonth->setGeometry(10, 155, 120, 300);
  bgMonth->setTitle(i18n("Months"));

  for (int mo = 1; mo <= 12; mo++)
  {
    cbMonth[mo] = new QCheckBox(bgMonth, "cbMonth");
    cbMonth[mo]->setGeometry(10, (mo-1)*23+15, 100, 25);
    cbMonth[mo]->setText(cttask->month.getName(mo).c_str());
    cbMonth[mo]->setChecked(cttask->month.get(mo));
    if (!cttask->month.get(mo)) everyDay = false;
  }

  // days of the month
  bgDayOfMonth = new QButtonGroup(this, "bgDayOfMonth");
  bgDayOfMonth->setGeometry(140, 155, 160, 130);
  bgDayOfMonth->setTitle(i18n("Days of the Month"));

  QPushButton* day;
  QString tmp;

  for (int dm = 1; dm <= 31; dm++)
  {
    day = new QPushButton(bgDayOfMonth);
    day->setGeometry(10+((dm-1)%7)*20,
      10+(dm-1)/7*20+10,20,20);
    day->setText(tmp.setNum(dm));
    day->setToggleButton(true);
    day->setOn(cttask->dayOfMonth.get(dm));
    pbDayOfMonth[dm] = day;
    if (!cttask->dayOfMonth.get(dm)) everyDay = false;
  }

  // days of the week
  bgDayOfWeek = new QButtonGroup(this, "bgDayOfWeek");
  bgDayOfWeek->setGeometry(140, 290, 160, 165);
  bgDayOfWeek->setTitle(i18n("Days of the Week"));

  for (int dw = 1; dw <= 7; dw++)
  {
    cbDayOfWeek[dw] = new QCheckBox(bgDayOfWeek);
    cbDayOfWeek[dw]->setGeometry(10, 20*(dw-1)+15, 140, 25);
    cbDayOfWeek[dw]->setText(cttask->dayOfWeek.getName(dw).c_str());
    cbDayOfWeek[dw]->setChecked(cttask->dayOfWeek.get(dw));
    if (!cttask->dayOfWeek.get(dw)) everyDay = false;
  }

  // daily
  bgEveryDay = new QButtonGroup(this, "bgEveryDay");
  bgEveryDay->setGeometry(310, 155, 140, 47);
  bgEveryDay->setTitle(i18n("Daily"));

  cbEveryDay = new QCheckBox(bgEveryDay, "cbEveryDay");
  cbEveryDay->setGeometry(10, 16, 120, 25);
  cbEveryDay->setText(i18n("Run Every Day"));
  cbEveryDay->setChecked(everyDay);

  // hours
  bgHour = new QButtonGroup(this, "bgHour");
  bgHour->setGeometry(310, 205, 140, 165);
  bgHour->setTitle(i18n("Hours"));

  labAM = new QLabel(bgHour, "labAM");
  labAM->setGeometry(90, 15, 40, 20);
  labAM->setAlignment(AlignRight | AlignVCenter);
  labAM->setText(i18n("AM"));

  labPM = new QLabel(bgHour, "labPM");
  labPM->setGeometry(90, 86, 40, 20);
  labPM->setAlignment(AlignRight | AlignVCenter);
  labPM->setText(i18n("PM"));

  for (int ho = 0; ho <= 23; ho++)
  {
    pbHour[ho] = new QPushButton(bgHour);
    pbHour[ho]->setText(tmp.setNum(ho));
    pbHour[ho]->setToggleButton(true);
    pbHour[ho]->setOn(cttask->hour.get(ho));
  }

  for (int ho1 = 0; ho1 <= 5; ho1++)
  {
    pbHour[ho1]->setGeometry(ho1*20+10, 40, 20, 20);
    pbHour[ho1+6]->setGeometry(ho1*20+10, 60, 20, 20);
    pbHour[ho1+12]->setGeometry(ho1*20+10, 111, 20, 20);
    pbHour[ho1+18]->setGeometry(ho1*20+10, 131, 20, 20);
  }

  // minutes
  bgMinute = new QButtonGroup(this, "bgMinute");
  bgMinute->setGeometry(310, 375, 140, 80);
  bgMinute->setTitle(i18n("Minutes"));

  for (int mi = 0; mi <= 55; mi+=5)
  {
    pbMinute[mi] = new QPushButton(bgMinute);
    pbMinute[mi]->setText(tmp.setNum(mi));
    pbMinute[mi]->setToggleButton(true);
    pbMinute[mi]->setOn(cttask->minute.get(mi));
  }

  for (int mi1 = 0; mi1 <= 25; mi1+=5)
  {
    pbMinute[mi1]->setGeometry(mi1*4+10, 25, 20, 20);
    pbMinute[mi1+30]->setGeometry(mi1*4+10, 45, 20, 20);
  }

  // OK
  pbOk = new QPushButton(i18n("&OK"), this, "pbOk");
  pbOk->setGeometry(280, 465, 80, 25);
  pbOk->setDefault(true);

  // Cancel
  pbCancel = new QPushButton(i18n("&Cancel"), this, "pbCancel");
  pbCancel->setGeometry(370, 465, 80, 25);

  // window
  setIcon(KTIcon::application(true));
  setCaption(i18n("Edit Task"));
  setFixedSize(460, 500);

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
    }
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setOn(true);
    }
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(true);
    }
  }
}

void KTTask::slotOK()
{
  // validate
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
    QMessageBox::information(this,
      kapp->caption(), message);
    return;
  }

  // make sure the file name is a good one if we have an
  // absolute path

  QString qs(leCommand->text());
  if (qs.find("/") == 0)
  {
    int endPos(qs.findRev(" ", qs.length()));
    if (endPos < 0) endPos = qs.length();

    QString programName(qs.left(endPos));

    QFileInfo file(programName);

    if (!file.isReadable())
    {
      QMessageBox::information(this,
        kapp->caption(),
        i18n("Can not locate program.  Please re-enter."));
      leCommand->setFocus();
      return;
    }

    if (!file.isExecutable())
    {
      QMessageBox::information(this,
        kapp->caption(),
        i18n("Program is not an executable file.  Please re-enter."));
      leCommand->setFocus();
      return;
    }
  }

  // save work in process
  if (cttask->user != "")
  {
    cttask->user = leUser->text();
  }
  else
  {
    cttask->user = "";
  }

  cttask->comment = leComment->text();
  cttask->command = leCommand->text();
  cttask->enabled = chkEnabled->isChecked();

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
  
  if(url.isLocalFile())
  {
    if(!url.isEmpty())
    {
      leCommand->setText((const char*)url.path());
    }
  }
  else
  {
    KMessageBox::sorry(0L,
      i18n("Only local or mounted files can be executed by crontab."));
  }
  
  leCommand->setFocus();
}
