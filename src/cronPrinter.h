/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>

class CTTask;
class CTVariable;
class CTCron;

class CronPrinter : public QObject
{
    Q_OBJECT

public:
    CronPrinter(QObject *parent);

public Q_SLOTS:
    void print(CTCron *cron);

private:
    QString getTasksTable(CTCron *cron);
    QString getVariablesTable(CTCron *cron);

    QString getRow(CTTask *task);
    QString getRow(CTVariable *variable);

    QString getTitle(CTCron *cron);
};
