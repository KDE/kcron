/*
    CT Cron Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctGlobalCron.h"

#include <KShell>
#include <KLocalizedString>

#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "logging.h"

CTGlobalCron::CTGlobalCron(CTHost *_ctHost)
    : CTCron()
{
    logDebug() << "Initializing CTGlobalCron";

    d->multiUserCron = true;
    d->systemCron = false;
    d->currentUserCron = false;

    d->userLogin = i18n("All users");

    mCtHost = _ctHost;
}

CTGlobalCron::~CTGlobalCron()
{
}

QList<CTTask *> CTGlobalCron::tasks() const
{
    logDebug() << "Global Cron Tasks";
    QList<CTTask *> tasks;

    const auto crons = mCtHost->mCrons;
    for (CTCron *cron : crons) {
        if (cron->isSystemCron()) {
            continue;
        }

        const auto ctasks = cron->tasks();

        for (CTTask *task : ctasks) {
            tasks.append(task);
        }
    }
    return tasks;
}

QList<CTVariable *> CTGlobalCron::variables() const
{
    logDebug() << "Global Cron Variables";
    QList<CTVariable *> variables;

    const auto crons = mCtHost->mCrons;
    for (CTCron *cron : crons) {
        if (cron->isSystemCron()) {
            continue;
        }
        const auto cvariables = cron->variables();
        for (CTVariable *variable : cvariables) {
            variables.append(variable);
        }
    }

    return variables;
}

void CTGlobalCron::addTask(CTTask *task)
{
    logDebug() << "Global Cron addTask";

    CTCron *actualCron = mCtHost->findUserCron(task->userLogin);
    actualCron->addTask(task);
}

void CTGlobalCron::addVariable(CTVariable *variable)
{
    logDebug() << "Global Cron addVariable";

    CTCron *actualCron = mCtHost->findUserCron(variable->userLogin);
    actualCron->addVariable(variable);
}

void CTGlobalCron::modifyTask(CTTask *task)
{
    logDebug() << "Global Cron modifyTask";

    CTCron *actualCron = mCtHost->findCronContaining(task);

    /*
     * actualCron could be NULL is the task came from clipboard because those tasks are never
     * linked to an existing CTCron* object
     */
    if (!actualCron || actualCron->userLogin() != task->userLogin) {
        if (actualCron) {
            actualCron->removeTask(task);
        }

        CTCron *newCron = mCtHost->findUserCron(task->userLogin);
        newCron->addTask(task);
    }
}

void CTGlobalCron::modifyVariable(CTVariable *variable)
{
    logDebug() << "Global Cron modifyVariable";

    CTCron *actualCron = mCtHost->findCronContaining(variable);

    /*
     * actualCron could be NULL is the task came from clipboard because those tasks are never
     * linked to an existing CTCron* object
     */
    if (!actualCron || actualCron->userLogin() != variable->userLogin) {
        if (actualCron) {
            actualCron->removeVariable(variable);
        }

        CTCron *newCron = mCtHost->findUserCron(variable->userLogin);
        newCron->addVariable(variable);
    }
}

void CTGlobalCron::removeTask(CTTask *task)
{
    logDebug() << "Global Cron removeTask";

    CTCron *actualCron = mCtHost->findCronContaining(task);
    actualCron->removeTask(task);
}

void CTGlobalCron::removeVariable(CTVariable *variable)
{
    logDebug() << "Global Cron removeVariable";

    CTCron *actualCron = mCtHost->findCronContaining(variable);
    actualCron->removeVariable(variable);
}
