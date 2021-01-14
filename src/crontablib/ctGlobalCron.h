/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CT_GLOBAL_CRON_H
#define CT_GLOBAL_CRON_H

#include <QString>
#include <QList>

#include "ctcron.h"

class CTTask;
class CTVariable;
class CTHost;

/**
 * A user (encapsulation of a single crontab file).  Encapsulates
 * file i/o, parsing, tokenization, and natural language description.
 */
class CTGlobalCron : public CTCron
{
public:

    explicit CTGlobalCron(CTHost *ctHost);

    /**
     * Destructor.
     */
    ~CTGlobalCron() override;

    QList<CTTask *> tasks() const override;

    QList<CTVariable *> variables() const override;

    void addTask(CTTask *task) override;
    void addVariable(CTVariable *variable) override;

    void modifyTask(CTTask *task) override;
    void modifyVariable(CTVariable *variable) override;

    void removeVariable(CTVariable *variable) override;
    void removeTask(CTTask *task) override;

private:
    CTHost *ctHost;
};

#endif // CT_GLOBAL_CRON_H
