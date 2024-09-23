/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <qqml.h>

class Task;
class TaskValidator : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString errorString READ errorString NOTIFY errorStringChanged FINAL)
    Q_PROPERTY(Task *task READ task WRITE setTask NOTIFY taskChanged FINAL)
    QML_ELEMENT

public:
    explicit TaskValidator(QObject *parent = nullptr) noexcept;
    ~TaskValidator() override;

    QString errorString() const noexcept;

    Task *task() const noexcept;
    void setTask(Task *task);

public Q_SLOTS:
    bool validate();

Q_SIGNALS:
    void errorStringChanged();
    void taskChanged();

private:
    bool validateCommand();
    bool validateMonth();
    bool validateDays();
    bool validateHours();
    bool validateMinutes();

private:
    QString mErrorString;
    QStringList mSpecialValidCommands;

    Task *mTask = nullptr;
};
