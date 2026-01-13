/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "task.h"

#include "cttask.h"

Task::Task(CTTask *ctTask, QObject *parent) noexcept
    : QObject(parent)
    , mTask(ctTask)
{
    Q_ASSERT(ctTask);
}

QString Task::comment() const noexcept
{
    return mTask->comment;
}

bool Task::setComment(const QString &comment) noexcept
{
    if (comment != mTask->comment) {
        mTask->comment = comment;
        Q_EMIT commentChanged();
        return true;
    }

    return false;
}

QString Task::command() const noexcept
{
    return mTask->command;
}

bool Task::setCommand(const QString &command) noexcept
{
    if (command != mTask->command) {
        mTask->command = command;
        Q_EMIT commandChanged();
        return true;
    }

    return false;
}

QString Task::userLogin() const noexcept
{
    return mTask->userLogin;
}

bool Task::setUserLogin(const QString &userLogin) noexcept
{
    if (userLogin != mTask->userLogin) {
        mTask->userLogin = userLogin;
        Q_EMIT userLoginChanged();
        return true;
    }

    return false;
}

bool Task::enabled() const noexcept
{
    return mTask->enabled;
}

bool Task::setEnabled(bool enabled) noexcept
{
    if (enabled != mTask->enabled) {
        mTask->enabled = enabled;
        Q_EMIT enabledChanged();
        return true;
    }

    return false;
}

bool Task::reboot() const noexcept
{
    return mTask->reboot;
}

bool Task::setReboot(bool reboot) noexcept
{
    if (reboot != mTask->reboot) {
        mTask->reboot = reboot;
        Q_EMIT rebootChanged();
        return true;
    }

    return false;
}

bool Task::systemCron() const noexcept
{
    return mTask->isSystemCrontab();
}

bool Task::setSystemCron(bool systemCron) noexcept
{
    if (systemCron != mTask->isSystemCrontab()) {
        mTask->setSystemCrontab(systemCron);
        Q_EMIT systemCronChanged();
        return true;
    }

    return false;
}

QString Task::exportTask() const
{
    return mTask->exportTask();
}

CTTask *Task::task() const noexcept
{
    return mTask;
}

void Task::updateTask(const CTTask &task) noexcept
{
    *mTask = task;
}

void Task::updateTask(CTTask *task) noexcept
{
    *mTask = *task;
}

void Task::apply()
{
    mTask->apply();
    Q_EMIT applied();
}

void Task::cancel()
{
    mTask->cancel();
    Q_EMIT canceled();
}

bool Task::hasChanges() const
{
    return mTask->dirty();
}

QString Task::schedulingCronFormat() const
{
    return mTask->schedulingCronFormat();
}

QString Task::describe() const
{
    return mTask->describe();
}

bool Task::isEveryDay() const
{
    return mTask->dayOfWeek.isAllEnabled() && mTask->month.isAllEnabled() && mTask->dayOfMonth.isAllEnabled();
}

bool Task::isDayOfMonthEnabled(int day) const
{
    return mTask->dayOfMonth.isEnabled(day);
}

bool Task::isMonthEnabled(int month) const
{
    return mTask->month.isEnabled(month);
}

bool Task::isWeekDayEnabled(int day) const
{
    return mTask->dayOfWeek.isEnabled(day);
}

bool Task::isMinuteEnabled(int minute) const
{
    return mTask->minute.isEnabled(minute);
}

bool Task::isHourEnabled(int hour) const
{
    return mTask->hour.isEnabled(hour);
}

void Task::setMonthEnabled(int month, bool enabled)
{
    mTask->month.setEnabled(month, enabled);
}

void Task::setDayOfMonthEnabled(int day, bool enabled)
{
    mTask->dayOfMonth.setEnabled(day, enabled);
}

void Task::setDayOfWeekEnabled(int day, bool enabled)
{
    mTask->dayOfWeek.setEnabled(day, enabled);
}

void Task::setHourEnabled(int hour, bool enabled)
{
    mTask->hour.setEnabled(hour, enabled);
}

void Task::setMinuteEnabled(int minute, bool enabled)
{
    mTask->minute.setEnabled(minute, enabled);
}

#include "task.moc"

#include "moc_task.cpp"
