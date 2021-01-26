/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctminute.h"

/**
 * Constructs from a tokenized string.
 */
CTMinute::CTMinute(const QString &tokStr)
    : CTUnit(0, 59, tokStr)
{
}

CTMinute::CTMinute()
    : CTUnit(0, 59, QLatin1String(""))
{
}

int CTMinute::findPeriod() const
{
    const QList<int> periods{1, 2, 5, 10, 15, 20, 30};

    return CTUnit::findPeriod(periods);
}

QString CTMinute::exportUnit() const
{
    const int period = findPeriod();
    if (period != 0 && period != 1) {
        return QStringLiteral("*/%1").arg(QString::number(period));
    }

    return CTUnit::exportUnit();
}
