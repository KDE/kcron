/*
    KT icons.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

/**
 * Crontab binary executable location
 * The $PATH variable could be used
 */
#define CRONTAB_BINARY "crontab"

#include <KQuickManagedConfigModule>

class CTHost;
class CTCron;
class TasksModel;
class Task;
class VariablesModel;
class Variable;
class CronPrinter;

class KCMCron : public KQuickManagedConfigModule
{
    Q_OBJECT

    Q_PROPERTY(TasksModel *tasksModel READ tasksModel CONSTANT)
    Q_PROPERTY(VariablesModel *variablesModel READ variablesModel CONSTANT)

    Q_PROPERTY(bool isPersonalUse READ isPersonalUse WRITE setIsPersonalUse NOTIFY isPersonalUseChanged)
    Q_PROPERTY(QStringList userList READ userList CONSTANT)

public:
    KCMCron(QObject *parent, const KPluginMetaData &data);
    ~KCMCron() override;

    Q_INVOKABLE void print();

    /**
     * Remove the excess pages before pushing new page
     */
    Q_INVOKABLE void refreshPages();

public Q_SLOTS:
    void load() override;
    void save() override;
    void defaults() override;

Q_SIGNALS:
    void isPersonalUseChanged();
    void showError(const QString &errorString, const QString &details = QString());
    void showOnboarding();

private Q_SLOTS:
    void addTask(Task *task);
    void addVariable(Variable *variable);

    void modifyTask(Task *task);
    void modifyVariable(Variable *variable);

    void removeTask(Task *task);
    void removeVariable(Variable *variable);

    void onMainUiReady();

private:
    /**
     * Additional init
     */
    void init();

    /**
     * Registers model roles for use in qml
     */
    void registerTypes();

    /**
     * Returns the current user's data if the "Personal Cron" button is selected, otherwise returns system data
     */
    CTCron *currentCron();

    /**
     * Complete update of data in task and variable models
     */
    void refreshCron();

    TasksModel *tasksModel() const noexcept;
    VariablesModel *variablesModel() const noexcept;

    bool isPersonalUse() const noexcept;
    void setIsPersonalUse(bool flag);

    QStringList userList() const noexcept;

private:
    CTHost *mCtHost = nullptr;
    TasksModel *const mTasksModel;
    VariablesModel *const mVariablesModel;
    CronPrinter *const mPrinter;

    bool mIsPersonalUse = true;
    QStringList mUserList;
};
