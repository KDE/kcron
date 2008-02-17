/***************************************************************************
 *   CT Month Implementation                                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ctmonth.h"
#include <klocale.h>

QList<QString> CTMonth::shortName;

CTMonth::CTMonth(const QString& tokStr) :
	CTUnit(1, 12, tokStr) {
}

QString CTMonth::describe() const {
	initializeNames();
	return (count() == 12) ? i18n("every month ") : CTUnit::genericDescribe(shortName);
}

QString CTMonth::getName(const int ndx) {
	initializeNames();
	return shortName[ndx];
}

void CTMonth::initializeNames() {
	if (shortName.isEmpty()) {
		shortName << "" << i18n("January") << i18n("February") << i18n("March") << i18n("April") << i18nc("May long", "May") << i18n("June") << i18nc("July long", "July") << i18n("August") << i18n("September") << i18n("October") << i18n("November") << i18n("December");
	}
}
