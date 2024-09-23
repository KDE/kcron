/*
    CT Month Header
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
 * Scheduled task months.
 */
class CTMonth : public CTUnit
{
public:
    /**
     * Constructs from a tokenized string.
     */
    explicit CTMonth(const QString &tokStr = QLatin1String(""));

    /**
     * Get natural language description.
     */
    virtual QString describe() const;

    /**
     * Get month name.
     */
    static QString getName(const int ndx);

    static const int MINIMUM = 1;
    static const int MAXIMUM = 12;

private:
    static void initializeNames();
    static QList<QString> shortName;
};
