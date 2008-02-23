/***************************************************************************
 *   CT Task Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTTASK_H
#define CTTASK_H

#include <QString>

#include "ctmonth.h"
#include "ctdom.h"
#include "ctdow.h"
#include "cthour.h"
#include "ctminute.h"

/**
 * A scheduled task (encapsulation of crontab entry).  Encapsulates
 * parsing, tokenization, and natural language description.
 */
class CTTask {
public:

	/**
	 * Constructs scheduled task from crontab format string.
	 */
	explicit CTTask(const QString& tokenString = "", const QString& _comment = "", bool syscron = false);

	/**
	 * Copy constructor.
	 */
	CTTask(const CTTask& source);

	/**
	 * Assignment operator.
	 */
	void operator =(const CTTask& source);

	/**
	 * Tokenizes scheduled task to crontab format.
	 */
	QString exportTask();
	
	/**
	 * Scheduling using the cron format
	 */
	QString schedulingCronFormat() const;

	/**
	 * Mark changes as applied.
	 */
	void apply();

	/**
	 * Cancel changes.
	 */
	void cancel();

	/**
	 * Indicates whether or not the task has been modified.
	 */
	bool dirty() const;

	/**
	 * Returns natural language description of the task's schedule.
	 */
	QString describe() const;

	/**
	 * Indicates whether or not the task belongs to the system crontab.
	 */
	bool isSystemCrontab() const;

	CTMonth month;
	CTDayOfMonth dayOfMonth;
	CTDayOfWeek dayOfWeek;
	CTHour hour;
	CTMinute minute;
	
	QString user;
	QString command;
	QString comment;
	
	bool enabled;
	bool reboot;

private:
	inline bool isSpace(const QString& token, int pos) {
		if (pos >= token.length())
			return false;

		if (token.at(pos) == ' ')
			return true;

		return false;
	}
	
	QString describeDayOfMonth() const;
	QString describeDayOfWeek() const;
	QString describeDateAndHours() const;
	
	QString createTimeFormat() const;
	QString createDateFormat() const;

	bool syscron;
	
	QString initialUser;
	QString initialCommand;
	QString initialComment;
	bool initialEnabled;
	bool initialReboot;
};

#endif // CTTASK_H
