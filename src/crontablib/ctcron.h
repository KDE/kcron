/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QList>
#include <QString>
#include <QStringList>

class CTTask;
class CTVariable;
class CTInitializationError;

struct passwd;

#include "ctSaveStatus.h"

class CommandLineStatus
{
public:
    int exitCode;

    QString commandLine;

    QString standardOutput;
    QString standardError;
};

class CommandLine
{
public:
    QString commandLine;

    QStringList parameters;

    QString standardOutputFile;

    CommandLineStatus execute();
};

class CTCronPrivate
{
public:
    /**
     * Indicates whether or not the crontab belongs to the system.
     */
    bool systemCron;

    /**
     * Indicates if this cron could have tasks and variables from different users.
     */
    bool multiUserCron;

    /**
     * Indicates whether or not the crontab belongs to the current user.
     */
    bool currentUserCron;

    /**
     * User login.
     */
    QString userLogin;

    /**
     * User real name.
     */
    QString userRealName;

    /**
     * User's scheduled tasks.
     */
    QList<CTTask *> task;

    /**
     * User's environment variables.  Note:  These are only environment variables
     * found in the user's crontab file and does not include any set in a
     * login or shell script such as ".bash_profile".
     */
    QList<CTVariable *> variable;

    int initialTaskCount;
    int initialVariableCount;

    CommandLine writeCommandLine;

    QString tmpFileName;

    /**
     * Contains path to the crontab binary file.
     */
    QString crontabBinary;
};

/**
 * A user (encapsulation of a single crontab file).  Encapsulates
 * file i/o, parsing, tokenization, and natural language description.
 */
class CTCron
{
public:
    /**
     * If you already have a struct passwd, use it instead.
     * This is never used for the system crontab.
     */
    explicit CTCron(const QString &cronBinary, const struct passwd *userInfos, bool currentUserCron, CTInitializationError &ctInitializationError);

    /**
     * Destructor.
     */
    virtual ~CTCron();

    /**
     * Copy one user's tasks and environment variables to another user.
     */
    CTCron &operator=(const CTCron &source);

    virtual QList<CTTask *> tasks() const;

    virtual QList<CTVariable *> variables() const;

    virtual void addTask(CTTask *task);
    virtual void addVariable(CTVariable *variable);

    virtual void modifyTask(CTTask *task);
    virtual void modifyVariable(CTVariable *variable);

    virtual void removeVariable(CTVariable *variable);
    virtual void removeTask(CTTask *task);

    /**
     * Tokenizes to crontab file format.
     */
    QString exportCron() const;

    /**
     * Apply changes.
     */
    CTSaveStatus save();

    /**
     * Cancel changes.
     */
    void cancel();

    /**
     * Indicates whether or not dirty.
     */
    bool isDirty() const;

    /**
     * Returns the PATH environment variable value.
     * A short cut to iterating the tasks vector.
     */
    QString path() const;

    /**
     * Returns true if this cron could have tasks and variables from a different user.
     */
    bool isMultiUserCron() const;

    /**
     * Returns true if this cron is the system cron.
     */
    bool isSystemCron() const;

    /**
     * Returns true if this cron is the cron of the user who launched this app.
     */
    bool isCurrentUserCron() const;

    QString userLogin() const;

    /**
     * TODO
     * Bugged method for the moment (need to parse x,x,x,x data from /etc/passwd).
     */
    QString userRealName() const;

protected:
    /**
     * Help constructor for subclasses.
     */
    explicit CTCron();

    // Initialize member variables from the struct passwd.
    bool initializeFromUserInfos(const struct passwd *userInfos);

private:
    /**
     * Can't copy a user!
     */
    CTCron(const CTCron &source);

    CTSaveStatus prepareSaveStatusError(const CommandLineStatus &commandLineStatus);

protected:
    /**
     * Parses crontab file format.
     */
    void parseFile(const QString &fileName);

    bool saveToFile(const QString &fileName);

    // d probably stands for data.
    CTCronPrivate *const d;
};

