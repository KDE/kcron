/*
    CT Minute Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CTMINUTE_H
#define CTMINUTE_H

#include "ctunit.h"

/**
 * Scheduled task minutes.
 */
class CTMinute : public CTUnit {
public:

	/**
	 * Constructs from a tokenized string.
	 */
	explicit CTMinute(const QString& tokStr);

	CTMinute();
	
	int findPeriod() const;
	
	QString exportUnit() const Q_DECL_OVERRIDE;

};

#endif // CTMINUTE_H
