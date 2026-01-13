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
class Task final : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY commentChanged FINAL)
    Q_PROPERTY(QString command READ command WRITE setCommand NOTIFY commandChanged FINAL)
    Q_PROPERTY(QString userLogin READ userLogin WRITE setUserLogin NOTIFY userLoginChanged FINAL)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged FINAL)
    Q_PROPERTY(bool reboot READ reboot WRITE setReboot NOTIFY rebootChanged FINAL)
    Q_PROPERTY(bool systemCron READ systemCron WRITE setSystemCron NOTIFY systemCronChanged FINAL)

public:
    explicit Task(CTTask *ctTask, QObject *parent) noexcept;
    ~Task() override = default;

    QString comment() const noexcept;
    bool setComment(const QString &comment) noexcept;

    QString command() const noexcept;
    bool setCommand(const QString &command) noexcept;

    QString userLogin() const noexcept;
    bool setUserLogin(const QString &userLogin) noexcept;

    bool enabled() const noexcept;
    bool setEnabled(bool enabled) noexcept;

    bool reboot() const noexcept;
    bool setReboot(bool reboot) noexcept;

    bool systemCron() const noexcept;
    bool setSystemCron(bool systemCron) noexcept;

    QString exportTask() const;

    CTTask *task() const noexcept;

    void updateTask(const CTTask &task) noexcept;
    void updateTask(CTTask *task) noexcept;

public:
    Q_INVOKABLE void apply();
    Q_INVOKABLE void cancel();

    Q_INVOKABLE bool hasChanges() const;

    Q_INVOKABLE QString schedulingCronFormat() const;
    Q_INVOKABLE QString describe() const;

    Q_INVOKABLE bool isEveryDay() const;

    Q_INVOKABLE bool isMonthEnabled(int month) const;
    Q_INVOKABLE bool isDayOfMonthEnabled(int day) const;
    Q_INVOKABLE bool isWeekDayEnabled(int day) const;
    Q_INVOKABLE bool isHourEnabled(int hour) const;
    Q_INVOKABLE bool isMinuteEnabled(int minute) const;

    Q_INVOKABLE void setMonthEnabled(int month, bool enabled);
    Q_INVOKABLE void setDayOfMonthEnabled(int day, bool enabled);
    Q_INVOKABLE void setDayOfWeekEnabled(int day, bool enabled);
    Q_INVOKABLE void setHourEnabled(int hour, bool enabled);
    Q_INVOKABLE void setMinuteEnabled(int minute, bool enabled);

Q_SIGNALS:
    void commentChanged();
    void commandChanged();
    void userLoginChanged();
    void enabledChanged();
    void rebootChanged();
    void systemCronChanged();

    void applied();
    void canceled();

private:
    CTTask *const mTask;
};
