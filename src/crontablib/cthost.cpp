/*
    --------------------------------------------------------------------
    CT Host Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cthost.h"

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h> // getuid()

#include <QFile>
#include <QTextStream>

#include <KLocalizedString>

#include "ctInitializationError.h"
#include "ctSystemCron.h"
#include "ctcron.h"

#include "logging.h"

CTHost::CTHost(const QString &cronBinary, CTInitializationError &ctInitializationError)
{
    struct passwd *userInfos = nullptr;

    mCrontabBinary = cronBinary;

    // If it is the root user
    if (getuid() == 0) {
        // Read /etc/passwd
        setpwent(); // restart
        while ((userInfos = getpwent())) {
            if (allowDeny(userInfos->pw_name)) {
                const QString errorMessage = createCTCron(userInfos);
                if (!errorMessage.isEmpty()) {
                    ctInitializationError.setErrorMessage(errorMessage);
                    return;
                }
            }
            // delete userInfos;
        }
        setpwent(); // restart again for others
    }
    // Non-root user, so just create user's cron table.
    else {
        // Get name from UID, check it against AllowDeny()
        unsigned int uid = getuid();
        setpwent(); // restart
        while ((userInfos = getpwent())) {
            if ((userInfos->pw_uid == uid) && (!allowDeny(userInfos->pw_name))) {
                ctInitializationError.setErrorMessage(
                    i18n("You have been blocked from using KCron\
	                      by either the /etc/cron.allow file or the /etc/cron.deny file.\
	                      \n\nCheck the crontab man page for further details."));

                return;
            }
            // delete userInfos;
        }

        setpwent(); // restart again for others

        passwd *currentUserPassword = getpwuid(uid);

        const QString errorMessage = createCTCron(currentUserPassword);
        if (!errorMessage.isEmpty()) {
            ctInitializationError.setErrorMessage(errorMessage);
            return;
        }

        // delete currentUserPassword;
    }

    // Create the system cron table.
    createSystemCron();
}

CTHost::~CTHost()
{
    qDeleteAll(mCrons);
}

bool CTHost::allowDeny(char *name)
{
    QFile allow(QStringLiteral("/etc/cron.allow"));

    // if cron.allow exists make sure user is listed
    if (allow.open(QFile::ReadOnly)) {
        QTextStream stream(&allow);
        while (!stream.atEnd()) {
            if (stream.readLine() == QLatin1String(name)) {
                allow.close();
                return true;
            }
        }
        allow.close();
        return false;
    } else {
        allow.close();
        QFile deny(QStringLiteral("/etc/cron.deny"));

        // else if cron.deny exists make sure user is not listed
        if (deny.open(QFile::ReadOnly)) {
            QTextStream stream(&deny);
            while (!stream.atEnd()) {
                if (stream.readLine() == QLatin1String(name)) {
                    deny.close();
                    return false;
                }
            }
            deny.close();
            return true;
        } else {
            deny.close();
            return true;
        }
    }
}

CTSaveStatus CTHost::save()
{
    if (!isRootUser()) {
        logDebug() << "Save current user cron";
        CTCron *ctCron = findCurrentUserCron();

        return ctCron->save();
    }

    for (CTCron *ctCron : std::as_const(mCrons)) {
        const CTSaveStatus ctSaveStatus = ctCron->save();

        if (ctSaveStatus.isError()) {
            return CTSaveStatus(i18nc("User login: errorMessage", "User %1: %2", ctCron->userLogin(), ctSaveStatus.errorMessage()),
                                ctSaveStatus.detailErrorMessage());
        }
    }

    return CTSaveStatus();
}

void CTHost::cancel()
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        ctCron->cancel();
    }
}

bool CTHost::isDirty()
{
    bool isDirty = false;

    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->isDirty()) {
            isDirty = true;
        }
    }

    return isDirty;
}

CTCron *CTHost::createSystemCron()
{
    CTCron *p = new CTSystemCron(mCrontabBinary);

    mCrons.append(p);

    return p;
}

QString CTHost::createCTCron(const struct passwd *userInfos)
{
    bool currentUserCron = false;
    if (userInfos->pw_uid == getuid()) {
        currentUserCron = true;
    }

    CTInitializationError ctInitializationError;
    auto p = new CTCron(mCrontabBinary, userInfos, currentUserCron, ctInitializationError);
    if (ctInitializationError.hasErrorMessage()) {
        delete p;
        return ctInitializationError.errorMessage();
    }

    mCrons.append(p);

    return QString();
}

CTCron *CTHost::findCurrentUserCron() const
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->isCurrentUserCron()) {
            return ctCron;
        }
    }

    logDebug() << "Unable to find the current user Cron. Please report this bug and your crontab config to the developers";
    return nullptr;
}

CTCron *CTHost::findSystemCron() const
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->isMultiUserCron()) {
            return ctCron;
        }
    }

    logDebug() << "Unable to find the system Cron. Please report this bug and your crontab config to the developers";
    return nullptr;
}

CTCron *CTHost::findUserCron(const QString &userLogin) const
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->userLogin() == userLogin) {
            return ctCron;
        }
    }

    logDebug() << "Unable to find the user Cron " << userLogin << ". Please report this bug and your crontab config to the developers";
    return nullptr;
}

CTCron *CTHost::findCronContaining(CTTask *ctTask) const
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->tasks().contains(ctTask)) {
            return ctCron;
        }
    }

    logDebug() << "Unable to find the cron of this task. Please report this bug and your crontab config to the developers";
    return nullptr;
}

CTCron *CTHost::findCronContaining(CTVariable *ctVariable) const
{
    for (CTCron *ctCron : std::as_const(mCrons)) {
        if (ctCron->variables().contains(ctVariable)) {
            return ctCron;
        }
    }

    logDebug() << "Unable to find the cron of this variable. Please report this bug and your crontab config to the developers";
    return nullptr;
}

bool CTHost::isRootUser() const
{
    return getuid() == 0;
}
