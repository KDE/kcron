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

#include "cthour.h"

#include <klocale.h>

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
		return QString("*/%1").arg(QString::number(period));
	
	return CTUnit::exportUnit();
}
