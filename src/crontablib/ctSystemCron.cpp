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

#include <QTemporaryFile>

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

    QTemporaryFile tmp;
    tmp.open();
    d->tmpFileName = tmp.fileName();

    CommandLine readCommandLine;

    readCommandLine.commandLine = QStringLiteral("cat");
    readCommandLine.parameters << QStringLiteral("/etc/crontab");
    readCommandLine.standardOutputFile = d->tmpFileName;

    d->writeCommandLine.commandLine = QStringLiteral("cat");
    d->writeCommandLine.parameters << d->tmpFileName;
    d->writeCommandLine.standardOutputFile = QStringLiteral("/etc/crontab");

    d->userLogin = i18n("System Crontab");
    d->userRealName = d->userLogin;

    d->initialTaskCount = 0;
    d->initialVariableCount = 0;

    // Don't set error if it can't be read, it means the user
    // doesn't have a crontab.
    if (readCommandLine.execute().exitCode == 0) {
        this->parseFile(d->tmpFileName);
    }

    d->initialTaskCount = d->task.size();
    d->initialVariableCount = d->variable.size();
}

CTSystemCron::~CTSystemCron()
{
}
