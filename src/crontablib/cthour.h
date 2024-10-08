/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ctunit.h"

/**
 * Scheduled task hours.
 */
class CTHour : public CTUnit
{
public:
    /**
     * Constructs from a tokenized string.
     */
    explicit CTHour(const QString &tokStr = QLatin1String(""));

    int findPeriod() const;

    QString exportUnit() const override;
};
