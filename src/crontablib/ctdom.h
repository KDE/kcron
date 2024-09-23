/*
    CT Day of Month Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QList>
#include <QString>

#include "ctunit.h"

/**
 * Scheduled task days of month.
 */
class CTDayOfMonth : public CTUnit
{
public:
    /**
     * Constructs from a tokenized string.
     */
    explicit CTDayOfMonth(const QString &tokStr = QLatin1String(""));

    /**
     * Get natural language description.
     */
    virtual QString describe() const;

    /**
     * Get day of month name.
     */
    static QString getName(const int ndx);

    static const int MINIMUM = 1;
    static const int MAXIMUM = 31;

private:
    static void initializeNames();
    static QList<QString> shortName;
};
