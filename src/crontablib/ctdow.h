/*
    CT Day of Week Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CTDOW_H
#define CTDOW_H

#include <QList>
#include <QString>

#include "ctunit.h"

/**
 * Scheduled task days of week.
 */
class CTDayOfWeek : public CTUnit
{
public:
    /**
     * Constructs from a tokenized string.
     */
    explicit CTDayOfWeek(const QString &tokStr = QLatin1String(""));

    /**
     * Override initialize to support crontab using both 0 and 7 for
     * Sunday.
     */
    void initialize(const QString &tokStr = QLatin1String(""));

    /**
     * Get natural language description.
     */
    virtual QString describe() const;

    /**
     * Get day of week name.
     */
    static QString getName(const int ndx, const bool format = CTDayOfWeek::longFormat);

    static const int MINIMUM = 1;
    static const int MAXIMUM = 7;

private:
    static void initializeNames();
    static QList<QString> shortName;
    static QList<QString> longName;
};

#endif // CTDOW_H
