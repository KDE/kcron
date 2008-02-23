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

	// Skip over 'silence' if found... old option in vixie cron
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

	exportTask += schedulingCronFormat();
	exportTask += "\t";

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
	return (month.isDirty() || dayOfMonth.isDirty() || dayOfWeek.isDirty() || hour.isDirty() || minute.isDirty() || (user != initialUser) || (command != initialCommand) || (comment != initialComment) || (enabled != initialEnabled) || (reboot != initialReboot));
}

QString CTTask::schedulingCronFormat() const {
	if (reboot) {
		return "@reboot";
	}

	QString scheduling;

	scheduling += minute.exportUnit() + " ";
	scheduling += hour.exportUnit() + " ";
	scheduling += dayOfMonth.exportUnit() + " ";
	scheduling += month.exportUnit() + " ";
	scheduling += dayOfWeek.exportUnit();

	return scheduling;

}

/**
 * Of the whole program, this method is probably the trickiest.
 * 
 * This method creates the natural language description, such as
 * "At 1:00am, every Sun".
 * 
 * First, I declare some strings for holding what can be internationalized.
 * Note the tokens such as "MONTHS".  Translators should reuse these
 * tokens in their translations.  See README.translators for more
 * information.
 * 
 * Second, I get the descriptions from the component parts such as
 * days of the month.
 * 
 * Third, I get hour/minute time combinations.  Although a little bit
 * awkward, I use the tm struct and strftime from <time.h>.  This
 * way this code is portable across all Unixes.
 * 
 * Fourth, I know that "every day of the week" and "every day of the
 * month" simply makes "every day".
 * 
 * Fifth and finally I do tag substitution to create the natural language
 * description.
 * 
 */
QString CTTask::describe() const {

	if (reboot) {
		return i18n("At system startup");
	}

	QString dateFormat = createDateFormat();

	QString timeFormat = createTimeFormat();

	return i18nc("1:Time Description, 2:Date Description", "%1, %2", timeFormat, dateFormat);
}

QString CTTask::describeDayOfWeek() const {
	return i18nc("Every 'days of week'", "every %1", dayOfWeek.describe());
}

QString CTTask::describeDayOfMonth() const {
	return i18nc("'Days of month' of 'Months'", "%1 of %2", dayOfMonth.describe(), month.describe());
}

QString CTTask::createDateFormat() const {
	/*
	 * "* * *" means truly every day.
	 * Note: Languages may use different phrases to indicate
	 * every day of month versus every day of week.
	 */
	QString dateFormat;
	if ((dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM) && (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM)) {
		dateFormat = i18n("every day ");
	}
	// Day of month not specified, so use day of week.
	else if (dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM) {
		dateFormat = describeDayOfWeek();
	}
	// Day of week not specified, so use day of month.
	else if (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM) {
		dateFormat = describeDayOfMonth();
	}
	else {
		dateFormat = i18nc("1:Day of month, 2:Day of week", "%1 as well as %2", describeDayOfMonth(), describeDayOfWeek());
	}
	
	return dateFormat;
	
}

QString CTTask::describeDateAndHours() const {
	// Create time description.
	int total = minute.enabledCount() * hour.enabledCount();

	QString timeDesc = "";
	int count = 0;

	for (int h = 0; h <= 23; h++) {
		if (hour.isEnabled(h)) {
			for (int m = 0; m <= 59; m++) {
				if (minute.isEnabled(m) == true) {
					QString hourString;
					if (h<10)
						hourString = "0" + QString::number(h);
					else
						hourString = QString::number(h);

					QString minuteString;
					if (m<10)
						minuteString = "0" + QString::number(m);
					else
						minuteString = QString::number(m);

					QString tmpStr = i18nc("1:Hour, 2:Minute", "%1:%2", hourString, minuteString);

					timeDesc += tmpStr;
					count++;
					switch (total - count) {
					case 0:
						break;
					case 1:
						if (total > 2)
							timeDesc += i18n(", and ");
						else
							timeDesc += i18n(" and ");
						break;
					default:
						timeDesc += i18n(", ");
					}
				}
			}
		}
	}
	
	
	return i18nc("Hour::Minute list", "At %1", timeDesc);
	
}

QString CTTask::createTimeFormat() const {
	if (hour.isAllEnabled()) {
		int minutePeriod = minute.findPeriod();
		if (minutePeriod == 1)
			return i18n("Every minutes");
		else if (minutePeriod != 0)
			return i18n("Every %1 minutes", minutePeriod);
	}
	
	return describeDateAndHours();
}

bool CTTask::isSystemCrontab() const {
	return syscron;
}
