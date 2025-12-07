/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "taskvalidator.h"

#include <QFileInfo>
#include <QStandardPaths>

#include <KLocalizedString>

#include "cttask.h"
#include "task.h"

TaskValidator::TaskValidator(QObject *parent) noexcept
    : QObject(parent)
{
    // Initialize special valid commands
    mSpecialValidCommands << QStringLiteral("cd");
}

TaskValidator::~TaskValidator()
{
}

QString TaskValidator::errorString() const noexcept
{
    return mErrorString;
}

Task *TaskValidator::task() const noexcept
{
    return mTask;
}

void TaskValidator::setTask(Task *task)
{
    if (mTask) {
        QObject::disconnect(mTask, &Task::commandChanged, this, &TaskValidator::validate);
        QObject::disconnect(mTask, &Task::commentChanged, this, &TaskValidator::validate);
        QObject::disconnect(mTask, &Task::rebootChanged, this, &TaskValidator::validate);
        QObject::disconnect(mTask, &Task::userLoginChanged, this, &TaskValidator::validate);
    }

    if (!task) {
        return;
    }

    mTask = task;

    QObject::connect(mTask, &Task::commandChanged, this, &TaskValidator::validate);
    QObject::connect(mTask, &Task::commentChanged, this, &TaskValidator::validate);
    QObject::connect(mTask, &Task::rebootChanged, this, &TaskValidator::validate);
    QObject::connect(mTask, &Task::userLoginChanged, this, &TaskValidator::validate);

    Q_EMIT taskChanged();
    this->validate();
}

bool TaskValidator::validate()
{
    if (!mTask) {
        return false;
    }

    if (!this->validateCommand()) {
        return false;
    }

    if (mTask->reboot()) {
        mErrorString.clear();
        Q_EMIT errorStringChanged();
        return true;
    }

    if (!this->validateMonth()) {
        return false;
    }

    if (!this->validateDays()) {
        return false;
    }

    if (!this->validateHours()) {
        return false;
    }

    if (!this->validateMinutes()) {
        return false;
    }

    mErrorString.clear();
    Q_EMIT errorStringChanged();

    return true;
}

bool TaskValidator::validateCommand()
{
    if (mTask->task()->command.isEmpty()) {
        mErrorString = xi18nc("@info", "<message>Choose a program to run</message>");
        Q_EMIT errorStringChanged();
        return false;
    }

    QPair<QString, bool> commandQuoted = mTask->task()->unQuoteCommand();

    if (commandQuoted.first.isEmpty()) {
        mErrorString = xi18nc("@info", "<message>Please type a valid command line</message>");
        Q_EMIT errorStringChanged();
        return false;
    }

    QStringList pathCommand = mTask->task()->separatePathCommand(commandQuoted.first, commandQuoted.second);
    if (pathCommand.isEmpty()) {
        mErrorString = xi18nc("@info", "<message>Please type a valid command line</message>");
        Q_EMIT errorStringChanged();
        return false;
    }

    const QString &path = pathCommand.at(0);
    const QString &binaryCommand = pathCommand.at(1);

    // For absolute paths, check the file directly to provide specific error messages
    // (file not found vs not executable) rather than the generic "command not found".
    if (commandQuoted.first.startsWith(QLatin1Char('/'))) {
        const QFileInfo fileInfo(commandQuoted.first);
        if (fileInfo.exists()) {
            if (fileInfo.isExecutable()) {
                return true;
            } else {
                mErrorString = xi18nc("@info", "<message>File exists but is not executable: <filename>%1</filename></message>", commandQuoted.first);
                Q_EMIT errorStringChanged();
                return false;
            }
        } else {
            mErrorString = xi18nc("@info", "<message>File not found: <filename>%1</filename></message>", commandQuoted.first);
            Q_EMIT errorStringChanged();
            return false;
        }
    }

    // If not found via direct check, try the standard path resolution
    // When path is empty, search in system PATH (e.g., for commands like 'ls', 'cat').
    // When path is not empty, search only in the specified directory.
    QString foundPath;
    if (path.isEmpty()) {
        foundPath = QStandardPaths::findExecutable(binaryCommand);
    } else {
        foundPath = QStandardPaths::findExecutable(binaryCommand, QStringList() << path);
    }

    if (!foundPath.isEmpty() || mSpecialValidCommands.contains(binaryCommand)) {
        return true;
    }

    mErrorString = xi18nc("@info", "<message>Command not found: <command>%1</command></message>", binaryCommand);
    Q_EMIT errorStringChanged();
    return false;
}

bool TaskValidator::validateMonth()
{
    bool isValid = false;
    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        if (mTask->isMonthEnabled(mo)) {
            isValid = true;
            break;
        }
    }

    if (!isValid) {
        mErrorString = xi18nc("@info", "<message>Please select from the <b>Months</b> section</message>");
        Q_EMIT errorStringChanged();
    }

    return isValid;
}

bool TaskValidator::validateDays()
{
    bool isValid = false;
    for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
        if (mTask->isDayOfMonthEnabled(dm)) {
            isValid = true;
            break;
        }
    }

    for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
        if (mTask->isWeekDayEnabled(dw)) {
            isValid = true;
            break;
        }
    }

    if (!isValid) {
        mErrorString = xi18nc("@info", "<message>Please select from either the <b>Days of Month</b> or the <b>Days of Week</b> section</message>");
        Q_EMIT errorStringChanged();
    }

    return isValid;
}

bool TaskValidator::validateHours()
{
    bool isValid = false;
    for (int ho = 0; ho <= 23; ho++) {
        if (mTask->isHourEnabled(ho)) {
            isValid = true;
            break;
        }
    }

    if (!isValid) {
        mErrorString = xi18nc("@info", "<message>Please select from the <b>Hours</b> section</message>");
        Q_EMIT errorStringChanged();
    }

    return isValid;
}

bool TaskValidator::validateMinutes()
{
    bool isValid = false;
    for (int mi = 0; mi <= 59; ++mi) {
        if (mTask->isMinuteEnabled(mi)) {
            isValid = true;
            break;
        }
    }

    if (!isValid) {
        mErrorString = xi18nc("@info", "<message>Please select from the <b>Minutes</b> section</message>");
        Q_EMIT errorStringChanged();
    }

    return isValid;
}

#include "taskvalidator.moc"

#include "moc_taskvalidator.cpp"
