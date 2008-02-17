/***************************************************************************
 *   CT Unit of Time Interval Implementation                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctunit.h"

#include <klocale.h>

#include "logging.h"

CTUnit::CTUnit(int _min, int _max, const QString& tokStr) {
	min = _min;
	max = _max;
	initialize(tokStr);
}

CTUnit::CTUnit(const CTUnit& source) {
	min = source.min;
	max = source.max;

	initialEnabled.clear();
	enabled.clear();
	for (int i = 0; i <= max; i++) {
		initialEnabled.append(false);
		enabled.append(source.enabled.at(i));
	}

	initialTokStr = "";
	isDirty = true;
}

CTUnit::~CTUnit() {
}

void CTUnit::operator = (const CTUnit& unit) {
	min = unit.min;
	max = unit.max;

	enabled.clear();
	for (int i = 0; i <= max; i++) {
		enabled.append(unit.enabled[i]);
	}
	isDirty = true;
	return;
}

void CTUnit::initialize(const QString& tokStr) {
	enabled.clear();
	for (int i = 0; i <= max; i++) {
		enabled.append(false);
		initialEnabled.append(false);
	}

	parse(tokStr);

	for (int i = min; i <= max; i++) {
		initialEnabled[i] = enabled[i];
	}
	
	initialTokStr = tokStr;
	isDirty = false;

	return;
}


void CTUnit::parse(const QString& tokenString) {

	QString tokStr = tokenString;

	// subelement is that which is between commas
	QString subelement;
	int commapos, slashpos, dashpos;
	int beginat, endat, step;

	// loop through each subelement
	tokStr += ',';
	while ((commapos = tokStr.indexOf(",")) > 0) {
		subelement = tokStr.mid(0, commapos);

		// find "/" to determine step
		slashpos = subelement.indexOf("/");
		if (slashpos == -1) {
			step = 1;
			slashpos = subelement.length();
		} else {
			step = fieldToValue(subelement.mid(slashpos+1, subelement.length()-slashpos-1));
			if (step < 1)
				step = 1;
		}

		// find "=" to determine range
		dashpos = subelement.indexOf("-");
		if (dashpos == -1) {
			// deal with "*"
			if (subelement.mid(0, slashpos) == "*") {
				beginat = min;
				endat = max;
			} else {
				beginat = fieldToValue(subelement.mid(0, slashpos));
				endat = beginat;
			}
		} else {
			beginat = fieldToValue(subelement.mid(0, dashpos));
			endat = fieldToValue(subelement.mid(dashpos+1, slashpos-dashpos-1));
		}

		// ignore out of range
		if (beginat < 0)
			beginat = 0;
		if (endat > max)
			endat = max;

		// setup enabled
		for (int i = beginat; i <= endat; i+=step) {
			enabled[i] = true;
		}

		tokStr = tokStr.mid(commapos+1, tokStr.length()-commapos-1);
	}

	return;
}


QString CTUnit::exportUnit() {
	if (count() == (max - min + 1))
		return "*";

	return tokenize();
}

QString CTUnit::tokenize() const {
	if (!isDirty) {
		return initialTokStr;
	} else {
		int total(count());
		int count(0);
		int num(min);
		QString tmpStr;

		while (num <= max) {
			if (enabled[num]) {
				tmpStr += QString::number(num);
				if (++count < total)
					tmpStr += ',';
			}
			num++;
		}
		if (count == (max - min + 1))
			tmpStr = "*";

		return tmpStr;
	}
}

QString CTUnit::genericDescribe(const QList<QString>& label) const {
	int total(count());
	int count(0);
	QString tmpStr;
	for (int i = min; i <= max; i++) {
		if (enabled[i]) {
			tmpStr += label.at(i);
			count++;
			switch (total - count) {
			case 0:
				break;
			case 1:
				if (total > 2)
					tmpStr += i18n(",");
				tmpStr += i18n(" and ");
				break;
			default:
				tmpStr += i18n(", ");
				break;
			}
		}
	}
	return tmpStr;
}

int CTUnit::begin() {
	return min;
}

int CTUnit::end() {
	return max;
}

bool CTUnit::get(int pos) const {
	return enabled.at(pos);
}

void CTUnit::set(int pos, bool value) {
	enabled[pos] = value;
	isDirty = true;
	return;
}

void CTUnit::enable(int pos) {
	enabled[pos] = true;
	isDirty = true;
	return;
}

void CTUnit::disable(int pos) {
	enabled[pos] = false;
	isDirty = true;
	return;
}

bool CTUnit::dirty() const {
	return isDirty;
}

int CTUnit::count() const {
	int total(0);
	for (int i = min; i <= max; i++)
		total += (enabled[i] == true);
	return total;
}

void CTUnit::apply() {
	initialTokStr = tokenize();
	for (int i = min; i <= max; i++)
		initialEnabled[i] = enabled[i];
	isDirty = false;
	return;
}

void CTUnit::cancel() {
	for (int i = min; i <= max; i++)
		enabled[i] = initialEnabled[i];
	isDirty = false;
	return;
}

int CTUnit::fieldToValue(const QString& entry) const {
	QString lower = entry.toLower();

	// check for days
	QList<QString> days;
	days << "sun" << "mon" << "tue" << "wed" << "thu" << "fri" << "sat";

	int day = days.indexOf(lower);
	if (day != -1) {
		return day;
	}

	// check for months
	QList<QString> months;
	months << "" << "jan" << "feb" << "mar" << "apr" << "may" << "jun" << "jul" << "aug" << "sep" << "oct" << "nov" << "dec";

	int month = months.indexOf(lower);
	if (month != -1) {
		return month;
	}

	//If the string does not match a day ora month, then it's a simple number (minute, hour or day of month)
	return entry.toInt();
}
