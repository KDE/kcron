/*
    CT Host Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QList>
#include <QString>

#include "ctSaveStatus.h"

class CTTask;
class CTVariable;
class CTCron;
class CTInitializationError;

struct passwd;

/**
 * The host machine, or computer (encapsulation of crontab files on the
 * host).
 *
 * If the user is the root user, the cron vector will have a member for
 * each user of the host plus one for the system crontab.
 *
 * If the user is a non-root user, there will be only one member in the
 * cron vector.
 */
class CTHost
{
public:
    /**
     * Constructs the user(s), scheduled tasks, and environment variables
     * from crontab files.
     */
    CTHost(const QString &cronBinary, CTInitializationError &ctInitializationError);

    /**
     * Destroys the user(s), scheduled tasks, and environment variable
     * objects.  Does not make any changes to the crontab files.  Any unapplied
     * changes are consequently "cancelled."
     */
    ~CTHost();

    /**
     * Apply changes.
     * return an empty string if no problem occurred.
     */
    CTSaveStatus save(CTCron *ctCron);

    /**
     * Cancel changes.
     */
    void cancel();

    /**
     * Indicates whether or not dirty.
     */
    bool isDirty();

    CTCron *findCurrentUserCron() const;
    CTCron *findSystemCron() const;
    CTCron *findUserCron(const QString &userLogin) const;

    CTCron *findCronContaining(CTTask *ctTask) const;
    CTCron *findCronContaining(CTVariable *ctVariable) const;

    /**
     * User(s).
     *
     * If the user is the root user, the cron vector will have a member for
     * each user of the host plus one for the system crontab.
     *
     * If the user is a non-root user, there will be only one member in the
     * cron vector.
     */
    QList<CTCron *> mCrons;

private:
    /**
     * Copy construction not allowed.
     */
    CTHost(const CTHost &source);

    /**
     * Assignment not allowed
     */
    CTHost &operator=(const CTHost &source);

    /**
     * Factory create a cron table.  Appends to the end of cron.
     */
    CTCron *createSystemCron();
    QString createCTCron(const struct passwd *password);

    /**
     * Check /etc/cron.allow, /etc/cron.deny
     */
    bool allowDeny(char *name);

    QString mCrontabBinary;
};
