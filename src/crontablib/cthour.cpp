/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cthour.h"

#include <KLocalizedString>

/**
 * Constructs from a tokenized string.
 */
CTHour::CTHour(const QString& tokStr) :
	CTUnit(0, 23, tokStr) {
}


int CTHour::findPeriod() const {
	QList<int> periods;
	periods << 2 << 3 << 4 << 6 << 8;

	return CTUnit::findPeriod(periods);
}

QString CTHour::exportUnit() const {
	int period = findPeriod();
	if (period!=0 && period!=1)
		return QStringLiteral( "*/%1" ).arg(QString::number(period));

	return CTUnit::exportUnit();
}
