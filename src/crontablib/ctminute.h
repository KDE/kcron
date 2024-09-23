/*
    CT Minute Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ctunit.h"

/**
 * Scheduled task minutes.
 */
class CTMinute : public CTUnit
{
public:
    /**
     * Constructs from a tokenized string.
     */
    explicit CTMinute(const QString &tokStr);

    CTMinute();

    int findPeriod() const;

    QString exportUnit() const override;
};
