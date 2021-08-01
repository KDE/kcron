/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QString>

#include "ctcron.h"

class CTSystemCron : public CTCron
{
public:
    /**
     * Constructs the scheduled tasks, environment variables from crontab
     * files and obtains some information about the user from the system.
     *
     * Default is to construct from the user's crontab.  Can also be called,
     * passing TRUE, to construct from the system crontab.  Throws an
     * exception if the crontab file can not be found, read, or parsed.
     */
    explicit CTSystemCron(const QString &cronBinary);

    /**
     * Destructor.
     */
    ~CTSystemCron() override;
};

