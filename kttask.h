/***************************************************************************
 *   KT task editor window header                                          *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTTASK_H
#define KTTASK_H

#include <config.h>

#include <kdialog.h>

class QLabel;
class QLineEdit;
class QCheckBox;
class QButtonGroup;
class QPushButton;
class KAccel;

class CTTask;

/**
  * Task editor window.
  */
class KTTask : public KDialog
{
  Q_OBJECT

public:

/**
  * Initialize from CTTask.
  */
  KTTask(CTTask* _cttask = 0, const QString &_caption="");

/**
  * Destroy.
  */
  ~KTTask();

private slots:

/**
  * Browse for command file.
  */
  void slotBrowse();

/**
  * Command has been changed.  Refresh icon.
  */
  void slotCommandChanged();

/**
  * Daily checkbox has been changed.
  */
  void slotDailyChanged();

/**
  * Apply changes and close.
  */
  void slotOK();

/**
  * Cancel and close.
  */
  void slotCancel();

/**
  * Set or clear all month checkboxes
  */
  void slotAllMonths();
  
/**
  * A month checkbox has changed
  */
  void slotMonthChanged();
  
/**
  * Set or clear all day of month checkboxes
  */
  void slotAllDaysOfMonth();
  
/**
  * A day of month checkbox has changed
  */
  void slotDayOfMonthChanged();
  
/**
  * Set or clear all day of week checkboxes
  */
  void slotAllDaysOfWeek();
  
/**
  * A day of week checkbox has changed
  */
  void slotDayOfWeekChanged();
  
/**
  * Set or clear all hour checkboxes
  */
  void slotAllHours();
  
/**
  * An hour checkbox has changed
  */
  void slotHourChanged();
  
/**
  * Set or clear all minute checkboxes
  */
  void slotAllMinutes();
  
/**
  * A minute checkbox has changed
  */
  void slotMinuteChanged();
  
private:

/**
  * Task.
  */
  CTTask*         cttask;

/**
  * Key accelerator.
  */

  KAccel*         key_accel;

  // Widgets.

  QLabel*         labUser;
  QLineEdit*      leUser;

  QLabel*         labIcon;

  QLabel*         labComment;
  QLineEdit*      leComment;

  QLabel*         labCommand;
  QLineEdit*      leCommand;
  QPushButton*    pbBrowse;

  QCheckBox*      chkEnabled;
  QCheckBox*      chkSilent;

  QButtonGroup*   bgMonth;
  QCheckBox*      cbMonth[13];
  QPushButton*    pbAllMonths;

  QButtonGroup*   bgDayOfMonth;
  QPushButton*    pbDayOfMonth[32];
  QPushButton*    pbAllDaysOfMonth;

  QButtonGroup*   bgDayOfWeek;
  QCheckBox*      cbDayOfWeek[8];
  QPushButton*    pbAllDaysOfWeek;

  QButtonGroup*   bgEveryDay;
  QCheckBox*      cbEveryDay;

  QButtonGroup*   bgHour;
  QLabel*         labAM;
  QLabel*         labPM;
  QPushButton*    pbHour[25];
  QPushButton*    pbAllHours;

  QButtonGroup*   bgMinute;
  QPushButton*    pbMinute[61];
  QPushButton*    pbAllMinutes;

  QPushButton*    pbOk;
  QPushButton*    pbCancel;

};

#endif // KTTASK_H
