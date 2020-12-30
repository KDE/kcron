/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctmonth.h"
#include <KLocalizedString>

QList<QString> CTMonth::shortName;

CTMonth::CTMonth(const QString& tokStr) :
	CTUnit(CTMonth::MINIMUM, CTMonth::MAXIMUM, tokStr) {
}

QString CTMonth::describe() const {
	initializeNames();
	return (enabledCount() == CTMonth::MAXIMUM) ? i18n("every month") : CTUnit::genericDescribe(shortName);
}

QString CTMonth::getName(const int ndx) {
	initializeNames();
	return shortName[ndx];
}

void CTMonth::initializeNames() {
	if (shortName.isEmpty()) {
		shortName << QLatin1String( "" ) << i18n("January") << i18n("February") << i18n("March") << i18n("April") << i18nc("May long", "May") << i18n("June") << i18nc("July long", "July") << i18n("August") << i18n("September") << i18n("October") << i18n("November") << i18n("December");
	}
}
