/***************************************************************************
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

#include <QLabel>

#include <qfileinfo.h>
#include <QLayout>
#include <QCheckBox>
#include <q3buttongroup.h>
#include <qpainter.h>
#include <qpalette.h>
//Added by qt3to4:
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "qstyleoption.h"
#include "qstylepainter.h"

#include <kacceleratormanager.h>
#include <klocale.h>
#include <klineedit.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <KStandardGuiItem>
#include <kdialog.h>
#include <kstandardshortcut.h>
#include <kstandarddirs.h>
#include <ktitlewidget.h>
#include <kiconloader.h>

#include "cttask.h"
#include "kticon.h"



class KTPushButton : public QPushButton
{
public:
   KTPushButton(QWidget * parent)
     : QPushButton(parent), isDirty(false)
   {
      updatePalette();
   }

   void updatePalette()
   {
      palNormal = ((QWidget *)parent())->palette();
      palSelected = palNormal;
      for(int cg = (int) QPalette::Active; cg < (int) QPalette::NColorGroups; cg++)
      {
        palSelected.setColor((QPalette::ColorGroup)cg, QColorGroup::Button,
                     palSelected.color((QPalette::ColorGroup)cg, QColorGroup::Highlight));
        palSelected.setColor((QPalette::ColorGroup)cg, QColorGroup::ButtonText,
                     palSelected.color((QPalette::ColorGroup)cg, QColorGroup::HighlightedText));
      }
      isDirty = true;
   }

   bool event( QEvent *e)
   {
     if (e->type() == QEvent::PaletteChange)
     {
        updatePalette();
        update();
     }
     return QPushButton::event(e);
   }

   void paintEvent(QPaintEvent*)
   {
     QStylePainter p(this);
     QStyleOptionButton option;
     initStyleOption(&option);

     if (isDirty || isChecked())
     {
       isDirty = false;
       if (isChecked())
       {
         option.palette = palSelected;
         QFont f=p.font();
         f.setBold(true);
         p.setFont(f);
       }
     }
   p.drawControl(QStyle::CE_PushButton, option);
   }

   bool isDirty;
   QPalette palSelected;
   QPalette palNormal;
};


KTTask::KTTask(CTTask* _cttask, const QString & _caption)
       :KDialog()
{
  cttask = _cttask;
  setModal(true);
  QWidget *main = new QWidget(this);
  setMainWidget(main);
  bool everyDay(true);
  QVBoxLayout *ml = new QVBoxLayout(main);
  ml->setSpacing(KDialog::spacingHint());

  QHBoxLayout *h0 = new QHBoxLayout();
  h0->setSpacing(KDialog::spacingHint());
  ml->addLayout(h0); 

  // top title widget
  titleWidget = new KTitleWidget(main);
  // don't call setupTitleWidget() - it references leCommand() which has not yet been declared
  h0->addWidget(titleWidget);

  QHBoxLayout *h1 = new QHBoxLayout();
  h1->setSpacing(KDialog::spacingHint());
  ml->addLayout(h1);

  // user
  labUser = new QLabel( i18n("&Run as:"), main);
  labUser->setObjectName("labUser");
  h1->addWidget( labUser );

  leUser = new KLineEdit(main );
  leUser->setObjectName( "leUser" );
  labUser->setBuddy(leUser);
  h1->addWidget( leUser );

  if (cttask->system())
  {
    leUser->setText(cttask->user);
  }
  else
  {
    labUser->hide();
    leUser->hide();
  }

  // comment
  QHBoxLayout *h2 = new QHBoxLayout();
  h2->setSpacing( KDialog::spacingHint() );
  ml->addLayout(h2);

  labComment = new QLabel( i18n("&Comment:"), main);
  labComment->setObjectName( "labComment" );
  h2->addWidget( labComment );

  leComment = new KLineEdit(main);
  leComment->setObjectName("leComment");
  labComment->setBuddy(leComment);
  h2->addWidget( leComment );

  leComment->setText(cttask->comment);

  // command
  QHBoxLayout *h3 = new QHBoxLayout();
  h3->setSpacing( KDialog::spacingHint() );
  ml->addLayout(h3);

  labCommand = new QLabel( i18n("&Program:"), main );
  labCommand->setObjectName( "labCommand" );
  h3->addWidget( labCommand );

  leCommand = new KLineEdit(main);
  leCommand->setObjectName( "leCommand" );
  labCommand->setBuddy(leCommand);
  h3->addWidget( leCommand );

  leCommand->setText(cttask->command);

  labComment->setFixedWidth( qMax( labComment->width(), labCommand->width()) );
  labCommand->setFixedWidth( qMax( labComment->width(), labCommand->width()) );

  pbBrowse = new QPushButton(main);
  pbBrowse->setObjectName("pbBrowse");
  pbBrowse->setText(i18n("&Browse..."));
  h3->addWidget( pbBrowse );

  QHBoxLayout *h3a = new QHBoxLayout();
  h3a->setSpacing( KDialog::spacingHint() );
  ml->addLayout(h3a);

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), main);
  chkEnabled->setObjectName("chkEnabled");
  chkEnabled->setChecked(cttask->enabled);
  h3a->addWidget( chkEnabled );

  // @reboot
  chkReboot = new QCheckBox(i18n("Run at system bootup"), main);
  chkReboot->setObjectName("chkReboot");
  chkReboot->setChecked(cttask->reboot);
  h3a->addWidget( chkReboot );

  QHBoxLayout *h4 = new QHBoxLayout();
  h4->setSpacing( KDialog::spacingHint() );
  ml->addLayout(h4);

  ml->addSpacing( 2 * KDialog::spacingHint() );

  // months
  bgMonth = new Q3ButtonGroup( i18n("Months"), main, "bgMonth");
  h4->addWidget( bgMonth );

  QVBoxLayout *vmonths = new QVBoxLayout( bgMonth );
  vmonths->setSpacing( KDialog::spacingHint() );
  vmonths->addSpacing( 2 * KDialog::spacingHint() );

  for (int mo = 1; mo <= 12; mo++)
  {
    cbMonth[mo] = new QCheckBox(bgMonth);
    cbMonth[mo]->setObjectName("cbMonth");
    cbMonth[mo]->setText(cttask->month.getName(mo));
    cbMonth[mo]->setChecked(cttask->month.get(mo));
    vmonths->addWidget( cbMonth[mo], Qt::AlignLeft );

    if (!cttask->month.get(mo)) everyDay = false;
  }
  pbAllMonths = new QPushButton(bgMonth);
  pbAllMonths->setObjectName("pbAllMonths");
  pbAllMonths->setText( i18n("Set All") );
  vmonths->addWidget( pbAllMonths, Qt::AlignLeft );

  QVBoxLayout *v1 = new QVBoxLayout();
  v1->setSpacing( KDialog::spacingHint() );
  h4->addLayout(v1);

  // days of the month
  bgDayOfMonth = new Q3ButtonGroup( i18n("Days of Month"), main, "bgDayOfMonth");
  v1->addWidget( bgDayOfMonth );

  QPushButton* day;
  QString tmp;

  QVBoxLayout *vdays = new QVBoxLayout( bgDayOfMonth );
  vdays->setSpacing( KDialog::spacingHint() );
  vdays->addSpacing( 2 * KDialog::spacingHint() );
  QHBoxLayout *hdays = 0;

  for (int dm = 1; dm <= 31; dm++)
  {
    if( (dm % 7) == 1 ) {
      hdays = new QHBoxLayout();
      hdays->setSpacing( KDialog::spacingHint() );
      vdays->addLayout(hdays);
    }

    day = new KTPushButton(bgDayOfMonth);
    KAcceleratorManager::setNoAccel(day);    
    day->setFixedSize(25, 25);
    day->setText(tmp.setNum(dm));
    day->setCheckable(true);
    day->setChecked(cttask->dayOfMonth.get(dm));
    pbDayOfMonth[dm] = day;
    if (!cttask->dayOfMonth.get(dm)) everyDay = false;

    hdays->addWidget( day, Qt::AlignLeft );
  }
  hdays->addStretch( 1 );
  pbAllDaysOfMonth = new QPushButton(bgDayOfMonth);
  pbAllDaysOfMonth->setObjectName("pbAllDaysOfMonth");
  pbAllDaysOfMonth->setText( i18n("Set All") );
  hdays->addWidget( pbAllDaysOfMonth, Qt::AlignLeft );

  // days of the week
  bgDayOfWeek = new Q3ButtonGroup( i18n("Days of Week"), main, "bgDayOfWeek");
  v1->addWidget( bgDayOfWeek );

  QVBoxLayout *v3 = new QVBoxLayout( bgDayOfWeek );
  v3->setSpacing( KDialog::spacingHint() );
  v3->addSpacing( 2 * KDialog::spacingHint() );

  for (int dw = 1; dw <= 7; dw++)
  {
    cbDayOfWeek[dw] = new QCheckBox(bgDayOfWeek);
    cbDayOfWeek[dw]->setText(cttask->dayOfWeek.getName(dw));
    cbDayOfWeek[dw]->setChecked(cttask->dayOfWeek.get(dw));
    v3->addWidget( cbDayOfWeek[dw] );

    if (!cttask->dayOfWeek.get(dw)) everyDay = false;
  }
  pbAllDaysOfWeek = new QPushButton(bgDayOfWeek);
  pbAllDaysOfWeek->setObjectName("pbAllDaysOfWeek");
  pbAllDaysOfWeek->setText( i18n("Set All") );
  v3->addWidget( pbAllDaysOfWeek, Qt::AlignLeft );

  QVBoxLayout *v2 = new QVBoxLayout();
  v2->setSpacing( KDialog::spacingHint() );
  h4->addLayout(v2);

  // daily
  bgEveryDay = new Q3ButtonGroup( i18nc("Every day of the week", "Daily"), main, "bgEveryDay");
  v2->addWidget( bgEveryDay );

  QVBoxLayout *v9 = new QVBoxLayout( bgEveryDay );
  v9->setSpacing( KDialog::spacingHint() );
  v9->addSpacing( 2 * KDialog::spacingHint() );

  cbEveryDay = new QCheckBox( i18n("Run every day"), bgEveryDay);
  cbEveryDay->setObjectName("cbEveryDay");
  cbEveryDay->setChecked(everyDay);
  v9->addWidget( cbEveryDay );

  // hours
  bgHour = new Q3ButtonGroup( i18n("Hours"), main, "bgHour");
  v2->addWidget( bgHour );
  QVBoxLayout *v4 = new QVBoxLayout( bgHour );
  v4->setSpacing( KDialog::spacingHint() );
  v4->addSpacing( 2 * KDialog::spacingHint() );

  labAM = new QLabel( i18n("AM"), bgHour);
  labAM->setObjectName("labAM");
  labAM->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  v4->addWidget( labAM );

  for (int ho = 0; ho <= 23; ho++)
  {
    pbHour[ho] = new KTPushButton(bgHour);
    KAcceleratorManager::setNoAccel(pbHour[ho]);  
    pbHour[ho]->setText(tmp.setNum(ho));
    pbHour[ho]->setCheckable(true);
    pbHour[ho]->setChecked(cttask->hour.get(ho));
    pbHour[ho]->setFixedSize(25, 25);
  }

  QHBoxLayout *hhours = new QHBoxLayout( );
  hhours->setSpacing( KDialog::spacingHint() );
  v4->addLayout(hhours);
  for (int ho1 = 0; ho1 <= 11; ho1++)
  {
    if( ho1 == 6 ) {
      hhours = new QHBoxLayout();
      hhours->setSpacing( KDialog::spacingHint() );
      v4->addLayout(hhours);
    }
    hhours->addWidget( pbHour[ho1] );
  }

  labPM = new QLabel( i18n("PM"), bgHour);
  labPM->setObjectName("labPM");
  labPM->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  v4->addWidget( labPM );

  hhours = new QHBoxLayout();
  hhours->setSpacing( KDialog::spacingHint() );
  v4->addLayout(hhours);
  for (int ho1 = 12; ho1 <= 23; ho1++)
  {
    if( ho1 == 18 ) {
      hhours = new QHBoxLayout();
      hhours->setSpacing( KDialog::spacingHint() );
      v4->addLayout(hhours);
    }

    hhours->addWidget( pbHour[ho1] );
  }

  hhours = new QHBoxLayout( );
  hhours->setSpacing( KDialog::spacingHint() );
  v4->addLayout(hhours);

  pbAllHours = new QPushButton(bgHour);
  pbAllHours->setObjectName("pbAllHours");
  pbAllHours->setText( i18n("Set All") );
  hhours->addWidget( pbAllHours, Qt::AlignLeft );

  // minutes
  bgMinute = new Q3ButtonGroup( i18n("Minutes"), main, "bgMinute");
  v2->addWidget( bgMinute );
  QVBoxLayout *vmin = new QVBoxLayout( bgMinute );
  vmin->setSpacing( KDialog::spacingHint() );
  vmin->addSpacing( 2 * KDialog::spacingHint() );

  for (int mi = 0; mi <= 55; mi+=5)
  {
    pbMinute[mi] = new KTPushButton(bgMinute);
    KAcceleratorManager::setNoAccel(pbMinute[mi]);  
    pbMinute[mi]->setText(tmp.setNum(mi));
    pbMinute[mi]->setCheckable(true);
    pbMinute[mi]->setChecked(cttask->minute.get(mi));
    pbMinute[mi]->setFixedSize(25, 25);
  }

  QHBoxLayout *hmin = new QHBoxLayout();
  hmin->setSpacing( KDialog::spacingHint() );
  vmin->addLayout(hmin);

  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    if( mi1 == 30 ) {
      hmin = new QHBoxLayout();
      hmin->setSpacing( KDialog::spacingHint() );
      vmin->addLayout(hmin);
    }

    hmin->addWidget( pbMinute[mi1] );
  }

  hmin = new QHBoxLayout();
  hmin->setSpacing( KDialog::spacingHint() );
  vmin->addLayout(hmin);

  pbAllMinutes = new QPushButton(bgMinute);
  pbAllMinutes->setText( i18n("Set All") );
  hmin->addWidget( pbAllMinutes, Qt::AlignLeft );

  // window
  setWindowIcon(KTIcon::application(KTIcon::Small));
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
  connect(pbBrowse, SIGNAL(clicked()), SLOT(slotWizard()));

  connect(leCommand, SIGNAL(textChanged(const QString&)),
    SLOT(slotWizard()));

  connect(chkEnabled, SIGNAL(clicked()), SLOT(slotEnabledChanged()));
  connect(chkEnabled, SIGNAL(clicked()), SLOT(slotWizard()));

  connect(chkReboot, SIGNAL(clicked()), SLOT(slotRebootChanged()));
  connect(chkReboot, SIGNAL(clicked()), SLOT(slotWizard()));

  connect(cbEveryDay, SIGNAL(clicked()), SLOT(slotDailyChanged()));
  connect(cbEveryDay, SIGNAL(clicked()), SLOT(slotWizard()));

  connect(this, SIGNAL(okClicked()), SLOT(slotOK()));
  connect(this, SIGNAL(cancelClicked()), SLOT(slotCancel()));

  connect(pbAllMonths, SIGNAL(clicked()), SLOT(slotAllMonths()));
  connect(pbAllMonths, SIGNAL(clicked()), SLOT(slotWizard()));

  for (int mo = 1; mo <= 12; mo++) {
    connect(cbMonth[mo], SIGNAL(clicked()), SLOT(slotMonthChanged()));
    connect(cbMonth[mo], SIGNAL(clicked()), SLOT(slotWizard()));
  }

  connect(pbAllDaysOfMonth, SIGNAL(clicked()), SLOT(slotAllDaysOfMonth()));
  connect(pbAllDaysOfMonth, SIGNAL(clicked()), SLOT(slotWizard()));

  for (int dm = 1; dm <= 31; dm++)
  {
    connect(pbDayOfMonth[dm], SIGNAL(clicked()), SLOT(slotDayOfMonthChanged()));
    connect(pbDayOfMonth[dm], SIGNAL(clicked()), SLOT(slotWizard()));
  }

  connect(pbAllDaysOfWeek, SIGNAL(clicked()), SLOT(slotAllDaysOfWeek()));
  connect(pbAllDaysOfWeek, SIGNAL(clicked()), SLOT(slotWizard()));

  for (int dw = 1; dw <= 7; dw++)
  {
    connect(cbDayOfWeek[dw], SIGNAL(clicked()), SLOT(slotDayOfWeekChanged()));
    connect(cbDayOfWeek[dw], SIGNAL(clicked()), SLOT(slotWizard()));
  }

  connect(pbAllHours, SIGNAL(clicked()), SLOT(slotAllHours()));
  connect(pbAllHours, SIGNAL(clicked()), SLOT(slotWizard()));

  for (int ho = 0; ho <= 23; ho++)
  {
    connect(pbHour[ho], SIGNAL(clicked()), SLOT(slotHourChanged()));
    connect(pbHour[ho], SIGNAL(clicked()), SLOT(slotWizard()));
  }

  connect(pbAllMinutes, SIGNAL(clicked()), SLOT(slotAllMinutes()));
  connect(pbAllMinutes, SIGNAL(clicked()), SLOT(slotWizard()));

  for (int mi = 0; mi <= 55; mi+=5)
  {
    connect(pbMinute[mi], SIGNAL(clicked()), SLOT(slotMinuteChanged()));
    connect(pbMinute[mi], SIGNAL(clicked()), SLOT(slotWizard()));
  }

  // key acceleration
  /*
  key_accel = new KAccel(this);
  key_accel->insert(KStandardShortcut::Open, this, SLOT(slotOK()));
  key_accel->insert(KStandardShortcut::Close, this, SLOT(slotCancel()));
  key_accel->insert(KStandardShortcut::Quit, this, SLOT(slotCancel()));
  key_accel->readSettings();
  */
  main->layout()->setSizeConstraint(QLayout::SetFixedSize);
  show();

  if (!chkEnabled->isChecked()) slotEnabledChanged();
  else if (chkReboot->isChecked()) slotRebootChanged(); 
  else if (cbEveryDay->isChecked()) slotDailyChanged(); 
  
  slotMonthChanged();
  slotDayOfMonthChanged();
  slotDayOfWeekChanged();
  slotHourChanged();
  slotMinuteChanged();

  slotWizard();
}

KTTask::~KTTask()
{
}

void KTTask::setupTitleWidget(const QString &comment)
{
  if (comment.isEmpty())	//krazy:exclude=duoblequote_chars
  {
    // try and get an icon for command
    QString qsCommand(leCommand->text());
  
    // qsCommand broken down this way to split off qsCommand attributes
    int firstSpace(qsCommand.indexOf(" "));
    if (firstSpace > 0) qsCommand = qsCommand.left(firstSpace);
    int lastSlash(qsCommand.lastIndexOf("/"));
    if (lastSlash > 0) qsCommand = qsCommand.right(qsCommand.size() - lastSlash - 1);

    // using KIconLoader() instead of getMaxIcon() because we need a null pixmap if pixmap cannot be found
    KIconLoader *loader = KIconLoader::global();
    QPixmap qpIcon(loader->loadIcon(qsCommand, KIconLoader::Desktop, 0, KIconLoader::DefaultState, QStringList(), 0L, true));
    if (qpIcon.isNull()) qpIcon = KTIcon::task(KTIcon::Large);

    titleWidget->setText(i18n("Add or modify a Kcron task"));  
    titleWidget->setComment(i18n("<i>This task has a valid configuration ...</i>"));
    titleWidget->setPixmap(qpIcon, KTitleWidget::ImageRight);
  }  
  else  
  {
    titleWidget->setText(i18n("Kcron Information"));  
    titleWidget->setComment(comment);
    titleWidget->setPixmap(KIcon(KTIcon::information(KTIcon::Large)), KTitleWidget::ImageRight);
  }
}

void KTTask::slotEnabledChanged()
{
  bool enabled = chkEnabled->isChecked();  
  labUser->setEnabled(enabled);
  leUser->setEnabled(enabled);
  leComment->setEnabled(enabled);
  labComment->setEnabled(enabled);
  leCommand->setEnabled(enabled);
  labCommand->setEnabled(enabled);
  pbBrowse->setEnabled(enabled);
  chkReboot->setEnabled(enabled);

  // if chkReboot is already checked, allow setEnabled(false) but not setEnable(true) ...
  if (!chkReboot->isChecked() || !enabled)
  {
    bgEveryDay->setEnabled(enabled);
    bgHour->setEnabled(enabled);
    bgMinute->setEnabled(enabled);
  }

  // if cbEveryDay is already checked, allow setEnabled(false) but not setEnable(true) ...
  if ((!chkReboot->isChecked() && !cbEveryDay->isChecked()) || !enabled)
  {
    bgMonth->setEnabled(enabled);
    bgDayOfMonth->setEnabled(enabled);
    bgDayOfWeek->setEnabled(enabled);
  }
}

void KTTask::slotRebootChanged()
{
  bool reboot = !chkReboot->isChecked(); 
  bgEveryDay->setEnabled(reboot);
  bgHour->setEnabled(reboot);
  bgMinute->setEnabled(reboot);

  // if cbEveryDay is already checked, bgMonth, bgDayOfMonth, bgDayOfWeek are already setEnable(flase)
  // so don't overide them ! ...
  if (!cbEveryDay->isChecked())
  {
    bgMonth->setEnabled(reboot);
    bgDayOfMonth->setEnabled(reboot);
    bgDayOfWeek->setEnabled(reboot);
  }
}

void KTTask::slotDailyChanged()
{
  if (cbEveryDay->isChecked())
  {
    for (int mo = 1; mo <= 12; mo++) cbMonth[mo]->setChecked(true);
    for (int dm = 1; dm <= 31; dm++) pbDayOfMonth[dm]->setChecked(true);
    for (int dw = 1; dw <= 7; dw++) cbDayOfWeek[dw]->setChecked(true);
    bgMonth->setEnabled(false);
    bgDayOfMonth->setEnabled(false);
    bgDayOfWeek->setEnabled(false);
    pbAllMonths->setEnabled(false);
    pbAllDaysOfMonth->setEnabled(false);
    pbAllDaysOfWeek->setEnabled(false);
  }
  else
  {
    bgMonth->setEnabled(true);
    bgDayOfMonth->setEnabled(true);
    bgDayOfWeek->setEnabled(true);
    pbAllMonths->setEnabled(true);
    pbAllDaysOfMonth->setEnabled(true);
    pbAllDaysOfWeek->setEnabled(true);
  }
  slotMonthChanged();
  slotDayOfMonthChanged();
  slotDayOfWeekChanged();
}

void KTTask::slotOK()      
{
  // Make it friendly for just selecting days of the month or
  // days of the week.

  int monthDaysSelected(0);
  for (int dm = 1; dm <= 31; dm++)
  {
    if (pbDayOfMonth[dm]->isChecked()) monthDaysSelected++;
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
      pbDayOfMonth[dm]->setChecked(true);
    }
  }

  if ((weekDaysSelected == 0) && (monthDaysSelected > 0))
  {
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(1);
    }
  }

  // save work in process
  if (!cttask->user.isEmpty())
  {
    cttask->user = (const char*)QFile::encodeName(leUser->text());
  }
  else
  {
    cttask->user = "";
  }

  cttask->comment = (const char *)leComment->text().toLocal8Bit();
  cttask->command = (const char *)leCommand->text().toLocal8Bit();
  cttask->enabled = chkEnabled->isChecked();
  cttask->reboot = chkReboot->isChecked();

  for (int mo = 1; mo <= 12; mo++)
  {
    cttask->month.set(mo, cbMonth[mo]->isChecked());
  }

  for (int dm = 1; dm <= 31; dm++)
  {
    cttask->dayOfMonth.set(dm, pbDayOfMonth[dm]->isChecked());
  }
  for (int dw = 1; dw <= 7; dw++)
  {
    cttask->dayOfWeek.set(dw, cbDayOfWeek[dw]->isChecked());
  }
  for (int ho = 0; ho <= 23; ho++)
  {
    cttask->hour.set(ho, pbHour[ho]->isChecked());
  }
  for (int mi = 0; mi <= 59; mi++)
  {
    cttask->minute.set(mi, false);
  }
  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    cttask->minute.set(mi1, pbMinute[mi1]->isChecked());
  }
  close();
}


void KTTask::slotWizard()
{
  bool error(false);

  if (!chkEnabled->isChecked())
  {
    setupTitleWidget(i18n("<i>Please check 'Enabled' to edit this task ...</i>"));
    KDialog::enableButtonOk(true);
    chkEnabled->setFocus();
    error = true;
  }

  if (chkReboot->isChecked() && !error)
  {
    setupTitleWidget(i18n("<i>This task will be run on system bootup ...</i>"));
    KDialog::enableButtonOk(true);
    error = true;
  }

  if (leCommand->text().isEmpty() && !error)
  {
    setupTitleWidget(i18n("<i>Please browse for a program to execute ...</i>"));
    KDialog::enableButtonOk(false);
    leCommand->setFocus();
    error = true;
  }  
  
  // make sure the file name is a good one if we have an
  // absolute path or if we are relying on $PATH
  QString pathstr, cmdstr, tmpstr;
  QString qs(leCommand->text());
  
  if (qs.left(0) == "/" or qs.left(1) == "/")
  {
    // detected a '/' at the start - must be absolute path 
    tmpstr = qs.section(' ', 0, 0);
    pathstr = tmpstr.section('/', 0, -2);
    cmdstr = tmpstr.section('/', -1);
  }  
  else
  {  
    // relying on $PATH
    pathstr = QString();  // null string
    cmdstr = qs.section(' ', 0, 0);
  } 

  bool found(false), exec(false);
  if (KStandardDirs::findExe(cmdstr, pathstr, KStandardDirs::IgnoreExecBit) != "") found = true;
  if (KStandardDirs::findExe(cmdstr, pathstr) != "") exec = true;
  
  if (found && !exec && !error) 
  {
    setupTitleWidget(i18n("<i>Please select an executable program ...</i>"));
    KDialog::enableButtonOk(false);
    leCommand->setFocus();
    error = true;
  }

  if (!found && !error)
  {
    setupTitleWidget(i18n("<i>Please browse for a program to execute ...</i>"));
    KDialog::enableButtonOk(false);
    leCommand->setFocus();
    error = true;
  }
  
  // the months
  bool valid(false);
  for (int mo = 1; mo <= 12; mo++)
  {
    if (cbMonth[mo]->isChecked()) valid = true;
  }
  if (!valid && !error)
  {
    setupTitleWidget(i18n("<i>Please select from the 'Months' section ...</i>"));
    KDialog::enableButtonOk(false);
    cbMonth[1]->setFocus();
    error = true;
  }

  // the days
  valid = false;
  for (int dm = 1; dm <= 31; dm++)
  {
    if (pbDayOfMonth[dm]->isChecked()) valid = true;
  }
  for (int dw = 1; dw <= 7; dw++)
  {
    if (cbDayOfWeek[dw]->isChecked()) valid = true;
  }

  if (!valid && !error)
  {
    setupTitleWidget(i18n("<i>Please select from either the 'Days of Month' or the 'Days of Week' section ...</i>"));
    KDialog::enableButtonOk(false);
    pbDayOfMonth[1]->setFocus();
    error = true;
  }

  // the hours
  valid = false;
  for (int ho = 0; ho <= 23; ho++)
  {
    if (pbHour[ho]->isChecked()) valid = true;
  }

  if (!valid && !error)
  {
    setupTitleWidget(i18n("<i>Please select from the 'Hours' section ...</i>"));
    KDialog::enableButtonOk(false);
    pbHour[0]->setFocus();
    error = true;
  }

  // the mins
  valid = false;
  for (int mi1 = 0; mi1 <= 55; mi1+=5)
  {
    if (pbMinute[mi1]->isChecked()) valid = true;
  }

  if (!valid && !error)
  {
    setupTitleWidget(i18n("<i>Please select from the 'Minutes' section ...</i>"));
    KDialog::enableButtonOk(false);
    pbMinute[0]->setFocus();
    error = true;
  }

  if (!error) 
  {
    setupTitleWidget();
    KDialog::enableButtonOk(true);
  }
}

void KTTask::slotCancel()
{
  close();
}

void KTTask::slotBrowse()
{
  KUrl url = KFileDialog::getOpenUrl();

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

void KTTask::slotAllMonths()
{
  if (pbAllMonths->text() == i18n("Set All")) {
    for (int mo = 1; mo <= 12; mo++)
    {
      cbMonth[mo]->setChecked(true);
    }
  }
  else {
    for (int mo = 1; mo <= 12; mo++)
    {
      cbMonth[mo]->setChecked(false);
    }
  }
  slotMonthChanged();
}

void KTTask::slotMonthChanged()
{
  bool allChecked = true;
  bool allCleared = true;
  for (int mo = 1; mo <= 12; mo++)
  {
    if (cbMonth[mo]->isChecked()) {
      allCleared = false;
    }
    else {
      allChecked = false;
    }
  }
  if (allCleared) {
    pbAllMonths->setText( i18n("Set All") );
  }
  else {
    pbAllMonths->setText( i18n("Clear All") );
  }
}

void KTTask::slotAllDaysOfMonth()
{
  if (pbAllDaysOfMonth->text() == i18n("Set All")) {
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setChecked(true);
    }
  }
  else {
    for (int dm = 1; dm <= 31; dm++)
    {
      pbDayOfMonth[dm]->setChecked(false);
    }
  }
  slotDayOfMonthChanged();
}

void KTTask::slotDayOfMonthChanged()
{
  bool allChecked = true;
  bool allCleared = true;
  for (int dm = 1; dm <= 31; dm++)
  {
    if (pbDayOfMonth[dm]->isChecked()) {
      allCleared = false;
    }
    else {
      allChecked = false;
    }
  }
  if (allCleared) {
    pbAllDaysOfMonth->setText( i18n("Set All") );
  }
  else {
    pbAllDaysOfMonth->setText( i18n("Clear All") );
  }
 }

void KTTask::slotAllDaysOfWeek()
{
  if (pbAllDaysOfWeek->text() == i18n("Set All")) {
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(true);
    }
  }
  else {
    for (int dw = 1; dw <= 7; dw++)
    {
      cbDayOfWeek[dw]->setChecked(false);
    }
  }
  slotDayOfWeekChanged();
}

void KTTask::slotDayOfWeekChanged()
{
  bool allChecked = true;
  bool allCleared = true;
  for (int dw = 1; dw <= 7; dw++)
  {
    if (cbDayOfWeek[dw]->isChecked()) {
      allCleared = false;
    }
    else {
      allChecked = false;
    }
  }
  if (allCleared) {
    pbAllDaysOfWeek->setText( i18n("Set All") );
  }
  else {
    pbAllDaysOfWeek->setText( i18n("Clear All") );
  }
 }

void KTTask::slotAllHours()
{
  if (pbAllHours->text() == i18n("Set All")) {
    for (int ho = 0; ho <= 23; ho++)
    {
      pbHour[ho]->setChecked(true);
    }
  }
  else {
    for (int ho = 0; ho <= 23; ho++)
    {
      pbHour[ho]->setChecked(false);
    }
  }
  slotHourChanged();
}

void KTTask::slotHourChanged()
{
  bool allChecked = true;
  bool allCleared = true;
  for (int ho = 0; ho <= 23; ho++)
  {
    if (pbHour[ho]->isChecked()) {
      allCleared = false;
    }
    else {
      allChecked = false;
    }
  }
  if (allCleared) {
    pbAllHours->setText( i18n("Set All") );
  }
  else {
    pbAllHours->setText( i18n("Clear All") );
  }
 }

void KTTask::slotAllMinutes()
{
  if (pbAllMinutes->text() == i18n("Set All")) {
    for (int mi = 0; mi <= 55; mi+=5)
    {
      pbMinute[mi]->setChecked(true);
    }
  }
  else {
    for (int mi = 0; mi <= 55; mi+=5)
    {
      pbMinute[mi]->setChecked(false);
    }
  }
  slotMinuteChanged();
}

void KTTask::slotMinuteChanged()
{
  bool allChecked = true;
  bool allCleared = true;
  for (int mi = 0; mi <= 55; mi+=5)
  {
    if (pbMinute[mi]->isChecked()) {
      allCleared = false;
    }
    else {
      allChecked = false;
    }
  }
  if (allCleared) {
    pbAllMinutes->setText( i18n("Set All") );
  }
  else {
    pbAllMinutes->setText( i18n("Clear All") );
  }
 }

#include "kttask.moc"
