/*
    CT Cron Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctcron.h"

#include <QDateTime>
#include <QFile>
#include <QProcess>
#include <QRegExp>
#include <QTemporaryFile>
#include <QTextStream>

#include <KLocalizedString>
#include <KShell>

#include "ctInitializationError.h"
#include "cttask.h"
#include "ctvariable.h"

// For root permissions
#include <KAuthAction>
#include <KAuthExecuteJob>

#include <pwd.h> // pwd, getpwnam(), getpwuid()
#include <unistd.h> // getuid(), unlink()

#include "kcm_cron_debug.h"

CommandLineStatus CommandLine::execute()
{
    QProcess process;

    int exitCode;
    process.start(commandLine, parameters);
    if (!process.waitForStarted()) {
        exitCode = 127;
    } else {
        process.waitForFinished(-1);
        exitCode = process.exitCode();
    }

    CommandLineStatus commandLineStatus;

    commandLineStatus.commandLine = commandLine + QLatin1String(" ") + parameters.join(QLatin1String(" "));

    commandLineStatus.standardOutput = QLatin1String(process.readAllStandardOutput());
    commandLineStatus.standardError = QLatin1String(process.readAllStandardError());
    commandLineStatus.exitCode = exitCode;

    return commandLineStatus;
}

CTCron::CTCron(const QString &crontabBinary, const struct passwd *userInfos, bool currentUserCron, CTInitializationError &ctInitializationError)
    : d(new CTCronPrivate())
{
    Q_ASSERT(userInfos);

    d->multiUserCron = false;
    d->systemCron = false;
    d->currentUserCron = currentUserCron;

    d->crontabBinary = crontabBinary;

    CommandLine readCommandLine;

    // regular user, so provide user's own crontab
    if (currentUserCron) {
        readCommandLine.commandLine = d->crontabBinary;
        readCommandLine.parameters << QStringLiteral("-l");
    } else {
        readCommandLine.commandLine = d->crontabBinary;
        readCommandLine.parameters << QStringLiteral("-u") << QLatin1String(userInfos->pw_name) << QStringLiteral("-l");
    }

    d->initialTaskCount = 0;
    d->initialVariableCount = 0;

    if (!initializeFromUserInfos(userInfos)) {
        ctInitializationError.setErrorMessage(i18n("No password entry found for uid '%1'", getuid()));
        qCDebug(KCM_CRON_LOG) << "Error in crontab creation of" << userInfos->pw_name;
        return;
    }

    // Don't set error if it can't be read, it means the user doesn't have a crontab.
    CommandLineStatus commandLineStatus = readCommandLine.execute();
    if (commandLineStatus.exitCode == 0) {
        QTextStream stream(&commandLineStatus.standardOutput);
        parseTextStream(&stream);
    } else {
        qCDebug(KCM_CRON_LOG) << "Error when executing command" << commandLineStatus.commandLine;
        qCDebug(KCM_CRON_LOG) << "Standard output :" << commandLineStatus.standardOutput;
        qCDebug(KCM_CRON_LOG) << "Standard error :" << commandLineStatus.standardError;
    }

    d->initialTaskCount = d->task.size();
    d->initialVariableCount = d->variable.size();
}

CTCron::CTCron()
    : d(new CTCronPrivate())
{
    // Empty constructor, new CTCronPrivate created.
}

bool CTCron::initializeFromUserInfos(const struct passwd *userInfos)
{
    if (userInfos == nullptr) {
        return false;
    } else {
        d->userLogin = QLatin1String(userInfos->pw_name);
        d->userRealName = QLatin1String(userInfos->pw_gecos);
        return true;
    }
}

CTCron &CTCron::operator=(const CTCron &source)
{
    if (this == &source) {
        return *this;
    }

    // Not sure when this would tigger.
    if (source.isSystemCron()) {
        qCDebug(KCM_CRON_LOG) << "Affect the system cron";
    }

    d->variable.clear();
    const auto variables = source.variables();
    for (CTVariable *ctVariable : variables) {
        auto tmp = new CTVariable(*ctVariable);
        d->variable.append(tmp);
    }

    d->task.clear();
    const auto tasks = source.tasks();
    for (CTTask *ctTask : tasks) {
        auto tmp = new CTTask(*ctTask);
        d->task.append(tmp);
    }

    return *this;
}

void CTCron::parseFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    parseTextStream(&in);
}

void CTCron::parseTextStream(QTextStream *stream)
{
    QString comment;
    bool leadingComment = true;

    while (!stream->atEnd()) {
        QString line = stream->readLine();

        // search for comments "#" but not disabled tasks "#\"
        if (line.indexOf(QLatin1String("#")) == 0 && line.indexOf(QLatin1String("\\")) != 1) {
            // Skip leading comments with leading spaces, those are not written by KCron
            if (leadingComment && line.startsWith(QLatin1String("# "))) {
                continue;
            }
            leadingComment = false;
            // If the first 10 characters don't contain a character, it's probably a disabled entry.
            int firstText = line.indexOf(QRegExp(QLatin1String("\\w")));
            if (firstText < 0) {
                continue;
            }

            if (firstText < 10) {
                // remove leading pound sign
                line = line.mid(1, line.length() - 1);
                if (comment.isEmpty()) {
                    comment = line.trimmed();
                } else {
                    comment += QLatin1String("\n") + line.trimmed();
                }
                continue;
            }
        }

        // either a task or a variable
        int firstWhiteSpace(line.indexOf(QRegExp(QLatin1String("[ \t]"))));
        int firstEquals(line.indexOf(QLatin1String("=")));

        // if there is an equals sign and either there is no
        // whitespace or the first whitespace is after the equals
        // sign, it must be a variable
        if ((firstEquals > 0) && ((firstWhiteSpace == -1) || firstWhiteSpace > firstEquals)) {
            // create variable
            auto tmp = new CTVariable(line, comment, d->userLogin);
            d->variable.append(tmp);
            comment.clear();
        }
        // must be a task, either enabled or disabled
        else {
            if (firstWhiteSpace > 0) {
                auto tmp = new CTTask(line, comment, d->userLogin, d->multiUserCron);
                d->task.append(tmp);
                comment.clear();
            }
        }
    }
}

QString CTCron::exportCron() const
{
    QString exportCron;

    for (CTVariable *ctVariable : std::as_const(d->variable)) {
        exportCron += ctVariable->exportVariable();
        exportCron += QLatin1String("\n");
    }

    for (CTTask *ctTask : std::as_const(d->task)) {
        exportCron += ctTask->exportTask();
        exportCron += QLatin1String("\n");
    }

    exportCron += QLatin1String("\n");
    QString exportInfo =
        i18nc("Generation Message + current date", "File generated by KCron the %1.", QDateTime::currentDateTime().toString(QLocale().dateTimeFormat()));
    exportCron += QLatin1String("# ") + exportInfo + QLatin1String("\n");

    return exportCron;
}

CTCron::~CTCron()
{
    qDeleteAll(d->task);

    qDeleteAll(d->variable);
    delete d;
}

CTSaveStatus CTCron::prepareSaveStatusError(const CommandLineStatus &commandLineStatus)
{
    QString standardOutput;
    if (commandLineStatus.standardOutput.isEmpty()) {
        standardOutput = i18n("<em>No output.</em>");
    } else {
        standardOutput = commandLineStatus.standardOutput;
    }

    QString standardError;
    if (commandLineStatus.standardError.isEmpty()) {
        standardError = i18n("<em>No error.</em>");
    } else {
        standardError = commandLineStatus.standardError;
    }

    QString detailError;
    if (commandLineStatus.exitCode == 127) {
        detailError = i18n("<p><strong>Command:</strong> %1</p><strong>Command could not be started</strong>", commandLineStatus.commandLine);
    } else {
        detailError = i18n("<p><strong>Command:</strong> %1</p><strong>Standard Output :</strong><pre>%2</pre><strong>Error Output :</strong><pre>%3</pre>",
                           commandLineStatus.commandLine,
                           standardOutput,
                           standardError);
    }

    return CTSaveStatus(i18n("An error occurred while updating crontab."), detailError);
}

CTSaveStatus CTCron::save()
{
    // write to temp file
    QTemporaryFile tmp;
    if (!tmp.open()) {
        return CTSaveStatus(i18n("Unable to open crontab file for writing"), i18n("The file %1 could not be opened.", tmp.fileName()));
    }

    {
        QTextStream out(&tmp);
        out << exportCron();
        out.flush();
    }
    tmp.close();

    // For root permissions.
    if (d->systemCron) {
        qCDebug(KCM_CRON_LOG) << "Attempting to save system cron";
        QVariantMap args;
        args.insert(QStringLiteral("source"), tmp.fileName());
        KAuth::Action saveAction(QStringLiteral("local.kcron.crontab.save"));
        saveAction.setHelperId(QStringLiteral("local.kcron.crontab"));
        saveAction.setArguments(args);
        KAuth::ExecuteJob *job = saveAction.execute();
        if (!job->exec())
            qCDebug(KCM_CRON_LOG) << "KAuth returned an error: " << job->error() << job->errorText();
        if (job->error() > 0) {
            return CTSaveStatus(i18n("KAuth::ExecuteJob Error"), job->errorText());
        }
    }
    // End root permissions.
    else {
        qCDebug(KCM_CRON_LOG) << "Attempting to save user cron";
        // Save without root permissions.
        CommandLine writeCommandLine;
        writeCommandLine.commandLine = d->crontabBinary;
        if (d->currentUserCron) {
            writeCommandLine.parameters << tmp.fileName();
        } else {
            writeCommandLine.parameters << QStringLiteral("-u") << d->userLogin << tmp.fileName();
        }

        const CommandLineStatus commandLineStatus = writeCommandLine.execute();
        if (commandLineStatus.exitCode != 0) {
            return prepareSaveStatusError(commandLineStatus);
        }
    }

    // Mark as applied
    for (CTTask *ctTask : std::as_const(d->task)) {
        ctTask->apply();
    }

    for (CTVariable *ctVariable : std::as_const(d->variable)) {
        ctVariable->apply();
    }

    d->initialTaskCount = d->task.size();
    d->initialVariableCount = d->variable.size();
    qCDebug(KCM_CRON_LOG) << "All saved";
    return CTSaveStatus();
}

void CTCron::cancel()
{
    const auto tasks = d->task;
    for (CTTask *ctTask : tasks) {
        ctTask->cancel();
    }

    const auto variables = d->variable;
    for (CTVariable *ctVariable : variables) {
        ctVariable->cancel();
    }
}

bool CTCron::isDirty() const
{
    if (d->initialTaskCount != d->task.count()) {
        return true;
    }

    if (d->initialVariableCount != d->variable.count()) {
        return true;
    }

    for (CTTask *ctTask : std::as_const(d->task)) {
        if (ctTask->dirty()) {
            return true;
        }
    }

    for (CTVariable *ctVariable : std::as_const(d->variable)) {
        if (ctVariable->dirty()) {
            return true;
        }
    }

    return false;
}

QString CTCron::path() const
{
    QString path;

    for (CTVariable *ctVariable : std::as_const(d->variable)) {
        if (ctVariable->variable == QLatin1String("PATH")) {
            path = ctVariable->value;
        }
    }

    return path;
}

QList<CTTask *> CTCron::tasks() const
{
    return d->task;
}

QList<CTVariable *> CTCron::variables() const
{
    return d->variable;
}

void CTCron::addTask(CTTask *task)
{
    if (isSystemCron()) {
        task->setSystemCrontab(true);
    } else {
        task->userLogin = d->userLogin;
        task->setSystemCrontab(false);
    }

    qCDebug(KCM_CRON_LOG) << "Adding task" << task->comment << " user : " << task->userLogin;

    d->task.append(task);
}

void CTCron::addVariable(CTVariable *variable)
{
    if (isSystemCron()) {
        variable->userLogin = QStringLiteral("root");
    } else {
        variable->userLogin = d->userLogin;
    }

    qCDebug(KCM_CRON_LOG) << "Adding variable" << variable->variable << " user : " << variable->userLogin;

    d->variable.append(variable);
}

void CTCron::modifyTask(CTTask * /*task*/)
{
    // Nothing to do specifically when modifying a task
}

void CTCron::modifyVariable(CTVariable * /*variable*/)
{
    // Nothing to do specifically when modifying a variable
}

void CTCron::removeTask(CTTask *task)
{
    d->task.removeAll(task);
}

void CTCron::removeVariable(CTVariable *variable)
{
    d->variable.removeAll(variable);
}

bool CTCron::isMultiUserCron() const
{
    return d->multiUserCron;
}

bool CTCron::isCurrentUserCron() const
{
    return d->currentUserCron;
}

bool CTCron::isSystemCron() const
{
    return d->systemCron;
}

QString CTCron::userLogin() const
{
    return d->userLogin;
}

QString CTCron::userRealName() const
{
    return d->userRealName;
}
