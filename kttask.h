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

#include <kdialog.h>
//Added by qt3to4:
#include <QLabel>

class QLabel;
class QLineEdit;
class QCheckBox;
class Q3ButtonGroup;
class QPushButton;

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
  * Override QDialog::accept ()
  */
  void accept(); 

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

  Q3ButtonGroup*   bgMonth;
  QCheckBox*      cbMonth[13];
  QPushButton*    pbAllMonths;

  Q3ButtonGroup*   bgDayOfMonth;
  QPushButton*    pbDayOfMonth[32];
  QPushButton*    pbAllDaysOfMonth;

  Q3ButtonGroup*   bgDayOfWeek;
  QCheckBox*      cbDayOfWeek[8];
  QPushButton*    pbAllDaysOfWeek;

  Q3ButtonGroup*   bgEveryDay;
  QCheckBox*      cbEveryDay;

  Q3ButtonGroup*   bgHour;
  QLabel*         labAM;
  QLabel*         labPM;
  QPushButton*    pbHour[25];
  QPushButton*    pbAllHours;

  Q3ButtonGroup*   bgMinute;
  QPushButton*    pbMinute[61];
  QPushButton*    pbAllMinutes;

};

  
#endif // KTTASK_H
