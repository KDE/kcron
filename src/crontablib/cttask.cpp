/***************************************************************************
 *   CT Task Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cttask.h"

#include <klocale.h>

#include <time.h>     // tm, strftime()
#include "logging.h"

CTTask::CTTask(const QString& tokenString, const QString& _comment, bool _syscron) :
	syscron(_syscron) {

	QString tokStr = tokenString;
	if (tokStr.mid(0, 2) == "#\\") {
		tokStr = tokStr.mid(2, tokStr.length() - 2);
		enabled = false;
	} else if (tokStr.mid(0, 1) == "#") {
		tokStr = tokStr.mid(1, tokStr.length() - 1);
		enabled = false;
	} else
		enabled = true;

	// Skip over 'silence' if found ... old option in vixie cron
	if (tokStr.mid(0, 1) == "-")
		tokStr = tokStr.mid(1, tokStr.length() - 1);

	reboot = false;
	if (tokStr.mid(0, 1) == "@") {
		if (tokStr.mid(1, 6) == "yearly") {
			tokStr = "0 0 1 1 *"+tokStr.mid(7, tokStr.length()-1);
		} else if (tokStr.mid(1, 8) == "annually") {
			tokStr = "0 0 1 1 *"+tokStr.mid(9, tokStr.length()-1);
		} else if (tokStr.mid(1, 7) == "monthly") {
			tokStr = "0 0 1 * *"+tokStr.mid(8, tokStr.length()-1);
		} else if (tokStr.mid(1, 6) == "weekly") {
			tokStr = "0 0 * * 0"+tokStr.mid(7, tokStr.length()-1);
		} else if (tokStr.mid(1, 5) == "daily") {
			tokStr = "0 0 * * *"+tokStr.mid(6, tokStr.length()-1);
		} else if (tokStr.mid(1, 6) == "hourly") {
			tokStr = "0 * * * *"+tokStr.mid(7, tokStr.length()-1);
		} else if (tokStr.mid(1, 6) == "reboot") {
			tokStr = tokStr.mid(7, tokStr.length()-1);
			reboot = true;
		}
	}

	int spacePos(tokStr.indexOf(QRegExp("[ \t]")));
	// If reboot bypass initialize functions so no keys selected in modify task
	if (reboot == false) {
		
		logDebug() << "Line : " << tokStr << endl;
		logDebug() << "MidLine : " << tokStr.mid(0, spacePos) << endl;
		minute.initialize(tokStr.mid(0, spacePos));

		while (isSpace(tokStr, spacePos+1))
			spacePos++;
		tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
		spacePos = tokStr.indexOf(QRegExp("[ \t]"));
		hour.initialize(tokStr.mid(0, spacePos));

		while (isSpace(tokStr, spacePos+1))
			spacePos++;
		tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
		spacePos = tokStr.indexOf(QRegExp("[ \t]"));
		dayOfMonth.initialize(tokStr.mid(0, spacePos));

		while (isSpace(tokStr, spacePos+1))
			spacePos++;
		tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
		spacePos = tokStr.indexOf(QRegExp("[ \t]"));
		month.initialize(tokStr.mid(0, spacePos));

		while (isSpace(tokStr, spacePos+1))
			spacePos++;
		tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
		spacePos = tokStr.indexOf(QRegExp("[ \t]"));
		dayOfWeek.initialize(tokStr.mid(0, spacePos));
	}

	if (syscron) {
		while (isSpace(tokStr, spacePos+1))
			spacePos++;
		tokStr = tokStr.mid(spacePos+1, tokStr.length()-1);
		spacePos = tokStr.indexOf(QRegExp("[ \t]"));
		user = tokStr.mid(0, spacePos);
	} else
		user = "";

	command = tokStr.mid(spacePos+1, tokStr.length()-1);
	// remove leading whitespace
	while (command.indexOf(QRegExp("[ \t]")) == 0)
		command = command.mid(1, command.length()-1);
	comment = _comment;

	initialUser = user;
	initialCommand = command;
	initialComment = comment;
	initialEnabled = enabled;
	initialReboot = reboot;
}

CTTask::CTTask(const CTTask &source) :
	month(source.month), dayOfMonth(source.dayOfMonth), dayOfWeek(source.dayOfWeek), hour(source.hour), minute(source.minute), user(source.user), command(source.command), comment(source.comment), enabled(source.enabled), reboot(source.reboot), initialUser(""), initialCommand(""),
			initialComment(""), initialEnabled(true), initialReboot(false) {
}

void CTTask::operator = (const CTTask& source) {
	month = source.month;
	dayOfMonth = source.dayOfMonth;
	dayOfWeek = source.dayOfWeek;
	hour = source.hour;
	minute = source.minute;
	user = source.user;
	command = source.command;
	comment = source.comment;
	enabled = source.enabled;
	reboot = source.reboot;
	initialUser = "";
	initialCommand = "";
	initialComment = "";
	initialEnabled = true;
	initialReboot = false;
	return;
}

QString CTTask::exportTask() {
	QString exportTask;

	//  if (task.comment != string(""))
	exportTask += "#" + comment + "\n";

	if (enabled == false)
		exportTask += "#\\";

	if (reboot) {
		exportTask += "@reboot\t";
	} else {
		exportTask += minute.exportUnit() + " ";
		exportTask += hour.exportUnit() + " ";
		exportTask += dayOfMonth.exportUnit() + " ";
		exportTask += month.exportUnit() + " ";
		exportTask += dayOfWeek.exportUnit() + "\t";
	}

	if (user != "")
		exportTask += user + "\t";

	exportTask += command + "\n";

	return exportTask;
}

void CTTask::apply() {
	month.apply();
	dayOfMonth.apply();
	dayOfWeek.apply();
	hour.apply();
	minute.apply();

	initialUser = user;
	initialCommand = command;
	initialComment = comment;
	initialEnabled = enabled;
	initialReboot = reboot;
}

void CTTask::cancel() {
	month.cancel();
	dayOfMonth.cancel();
	dayOfWeek.cancel();
	hour.cancel();
	minute.cancel();

	user = initialUser;
	command = initialCommand;
	comment = initialComment;
	enabled = initialEnabled;
	reboot = initialReboot;
}

bool CTTask::dirty() const {
	return (month.dirty() || dayOfMonth.dirty() || dayOfWeek.dirty() || hour.dirty() || minute.dirty() || (user != initialUser) || (command != initialCommand) || (comment != initialComment) || (enabled != initialEnabled) || (reboot != initialReboot));
}

QString CTTask::describe() const {

	// Of the whole program, this method is probably the trickiest.
	//
	// This method creates the natural language description, such as
	// "At 1:00am, every Sun".
	//
	// First, I declare some strings for holding what can be internationalized.
	// Note the tokens such as "MONTHS".  Translators should reuse these
	// tokens in their translations.  See README.translators for more
	// information.
	//
	// Second, I get the descriptions from the component parts such as
	// days of the month.
	//
	// Third, I get hour/minute time combinations.  Although a little bit
	// awkward, I use the tm struct and strftime from <time.h>.  This
	// way this code is portable across all Unixes.
	//
	// Fourth, I know that "every day of the week" and "every day of the
	// month" simply makes "every day".
	//
	// Fifth and finally I do tag substitution to create the natural language
	// description.

	QString tmFormat = i18n("%H:%M");
	QString DOMFormat = i18nc("Please translator, read the README.translators file in kcron's source code", "DAYS_OF_MONTH of MONTHS");
	QString DOWFormat = i18nc("Really, read that file", "every DAYS_OF_WEEK");
	QString dateFormat = i18n("DOM_FORMAT as well as DOW_FORMAT");
	QString timeFormat = i18n("At TIME");
	QString format = i18n("TIME_FORMAT, DATE_FORMAT");

	// Get natural language description of day of month,
	// month name, and day of week.

	QString DOMDesc(dayOfMonth.describe());
	QString monthDesc(month.describe());
	QString DOWDesc(dayOfWeek.describe());

	// Create time description.

	int total(minute.count()*hour.count());

	QString timeDesc("");
	int count(0);

	for (int h = 0; h <= 23; h++)
		if (hour.get(h))
			for (int m = 0; m <= 59; m++)
				if (minute.get(m)) {
					tm time;
					time.tm_sec = 0;
					time.tm_min = m;
					time.tm_hour = h;
					time.tm_mday = 0;
					time.tm_mon = 0;
					time.tm_year = 0;
					time.tm_wday = 0;
					time.tm_yday = 0;
					time.tm_isdst= 0;

					char tmp[12];
					strftime(tmp, 12, tmFormat.toLocal8Bit(), &time);
					QString tmpStr = tmp;

					// remove leading space
					if (tmpStr.mid(0, 1) == " ")
						tmpStr = tmpStr.mid(1, tmpStr.length()-1);

					timeDesc += tmpStr;
					count++;
					switch (total - count) {
					case 0:
						break;
					case 1:
						if (total > 2)
							timeDesc += (const char *)i18n(", and ").toLocal8Bit();
						else
							timeDesc += (const char *)i18n(" and ").toLocal8Bit();
						break;
					default:
						timeDesc += (const char*)i18n(", ").toLocal8Bit();
					}
				}

	// "* * *" means truly every day.
	// Note: Languages may use different phrases to indicate
	// every day of month versus every day of week.

	if ((dayOfMonth.count() == 31) && (dayOfWeek.count() == 7))
		dateFormat = (const char *)i18n("every day ").toLocal8Bit();
	else {
		// Day of month not specified, so use day of week.
		if (dayOfMonth.count() == 31)
			dateFormat = "DOW_FORMAT";
		// Day of week not specified, so use day of month.
		if (dayOfWeek.count() == 7)
			dateFormat = "DOM_FORMAT";
	}

	// Replace tags to build natural language description.

	int pos = 0;

	pos = DOMFormat.indexOf("DAYS_OF_MONTH");
	if (pos > -1)
		DOMFormat.replace(pos, 13, DOMDesc);

	pos = DOMFormat.indexOf("MONTHS");
	if (pos > -1)
		DOMFormat.replace(pos, 6, monthDesc);

	pos = DOWFormat.indexOf("DAYS_OF_WEEK");
	if (pos > -1)
		DOWFormat.replace(pos, 12, DOWDesc);

	pos = dateFormat.indexOf("DOM_FORMAT");
	if (pos > -1)
		dateFormat.replace(pos, 10, DOMFormat);

	pos = dateFormat.indexOf("DOW_FORMAT");
	if (pos > -1)
		dateFormat.replace(pos, 10, DOWFormat);

	pos = timeFormat.indexOf("TIME");
	if (pos > -1)
		timeFormat.replace(pos, 4, timeDesc);

	pos = format.indexOf("DATE_FORMAT");
	if (pos > -1)
		format.replace(pos, 11, dateFormat);

	pos = format.indexOf("TIME_FORMAT");
	if (pos > -1)
		format.replace(pos, 11, timeFormat);

	return format;
}

bool CTTask::system() const {
	return syscron;
}
