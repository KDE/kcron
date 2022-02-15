/*
    CT Cron Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctSystemCron.h"

#include <KLocalizedString>
#include <KShell>

#include <QFileInfo>

#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "kcm_cron_debug.h"

CTSystemCron::CTSystemCron(const QString &crontabBinary)
    : CTCron()
{
    d->systemCron = true;
    d->multiUserCron = true;
    d->currentUserCron = false;

    d->crontabBinary = crontabBinary;

    d->userLogin = i18n("root");
    d->userRealName = d->userLogin;

    d->initialTaskCount = 0;
    d->initialVariableCount = 0;

    // Don't set error if it can't be read, it means the user
    // doesn't have a crontab.
    const QString crontabFile = QStringLiteral("/etc/crontab");
    if (QFileInfo::exists(crontabFile)) {
        parseFile(crontabFile);
    }

    d->initialTaskCount = d->task.size();
    d->initialVariableCount = d->variable.size();
}

CTSystemCron::~CTSystemCron()
{
}
