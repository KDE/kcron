/*
    KT icons implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcmCron.h"

#include "cronPrinter.h"
#include "ctInitializationError.h"
#include "ctcron.h"
#include "cthost.h"
#include "task.h"
#include "tasksmodel.h"
#include "taskvalidator.h"
#include "variable.h"
#include "variablesmodel.h"

#include "kcm_cron_debug.h"

K_PLUGIN_FACTORY_WITH_JSON(KCMCronQmlFactory, "kcm_cron.json", registerPlugin<KCMCron>();)

KCMCron::KCMCron(QObject *parent, const KPluginMetaData &data)
    : KQuickManagedConfigModule(parent, data)
    , mTasksModel(new TasksModel(this))
    , mVariablesModel(new VariablesModel(this))
    , mPrinter(new CronPrinter(this))
{
    setButtons(Help | Apply | Default);
    registerTypes();
    init();

    QObject::connect(this, &KCMCron::mainUiReady, this, &KCMCron::onMainUiReady);

    QObject::connect(mVariablesModel, &VariablesModel::addVariable, this, &KCMCron::addVariable);
    QObject::connect(mVariablesModel, &VariablesModel::modifyVariable, this, &KCMCron::modifyVariable);
    QObject::connect(mVariablesModel, &VariablesModel::removeVariable, this, &KCMCron::removeVariable);

    QObject::connect(mTasksModel, &TasksModel::addTask, this, &KCMCron::addTask);
    QObject::connect(mTasksModel, &TasksModel::modifyTask, this, &KCMCron::modifyTask);
    QObject::connect(mTasksModel, &TasksModel::removeTask, this, &KCMCron::removeTask);
}

KCMCron::~KCMCron()
{
    delete mCtHost;
}

void KCMCron::print()
{
    qCWarning(KCM_CRON_LOG) << "Calling print";
    mPrinter->print(currentCron());
}

void KCMCron::load()
{
    qCDebug(KCM_CRON_LOG) << "Calling load";

    mCtHost->cancel();
}

void KCMCron::save()
{
    qCDebug(KCM_CRON_LOG) << "Saving crontab...";

    CTSaveStatus saveStatus = mCtHost->save(currentCron());
    if (saveStatus.isError()) {
        qCWarning(KCM_CRON_LOG) << saveStatus.errorMessage() << saveStatus.detailErrorMessage();
        Q_EMIT showError(saveStatus.errorMessage(), saveStatus.detailErrorMessage());
    }

    qCDebug(KCM_CRON_LOG) << "saved ct host";
}

void KCMCron::defaults()
{
    qCDebug(KCM_CRON_LOG) << "Loading defaults";

    mCtHost->cancel();
}

void KCMCron::addTask(Task *task)
{
    currentCron()->addTask(task->task());
    setNeedsSave(true);
}

void KCMCron::addVariable(Variable *variable)
{
    currentCron()->addVariable(variable->variable());
    setNeedsSave(true);
}

void KCMCron::modifyTask(Task *task)
{
    currentCron()->modifyTask(task->task());
    setNeedsSave(true);
}

void KCMCron::modifyVariable(Variable *variable)
{
    currentCron()->modifyVariable(variable->variable());
    setNeedsSave(true);
}

void KCMCron::removeTask(Task *task)
{
    currentCron()->removeTask(task->task());
    setNeedsSave(true);
}

void KCMCron::removeVariable(Variable *variable)
{
    currentCron()->removeVariable(variable->variable());
    setNeedsSave(true);
}

void KCMCron::onMainUiReady()
{
    // Display greeting screen.
    // If there currently are no scheduled tasks...
    int taskCount = 0;
    for (CTCron *ctCron : std::as_const(mCtHost->mCrons)) {
        taskCount += ctCron->tasks().count();
    }

    if (taskCount == 0) {
        Q_EMIT showOnboarding();
    }
}

void KCMCron::init()
{
    CTInitializationError ctInitializationError;
    mCtHost = new CTHost(QStringLiteral(CRONTAB_BINARY), ctInitializationError);

    if (ctInitializationError.hasErrorMessage()) {
        qCWarning(KCM_CRON_LOG) << "An error occurred while creating the CTHost object";
        qCWarning(KCM_CRON_LOG) << "Message:" << ctInitializationError.errorMessage();
        Q_EMIT showError(ctInitializationError.errorMessage());
    }

    for (CTCron *ctCron : std::as_const(mCtHost->mCrons)) {
        mUserList.append(ctCron->userLogin());
    }

    refreshCron();
}

void KCMCron::registerTypes()
{
    // @uri org.kde.private.kcms.cron
    const char uri[] = "org.kde.private.kcms.cron";

    qmlRegisterUncreatableType<VariablesModel>(uri, 1, 0, "VariablesModel", QStringLiteral("Cannot create instances of VariablesModel"));
    qmlRegisterUncreatableType<TasksModel>(uri, 1, 0, "TasksModel", QStringLiteral("Cannot create instances of TasksModel"));
    qmlRegisterType<TaskValidator>(uri, 1, 0, "TaskValidator");
}

CTCron *KCMCron::currentCron()
{
    if (mIsPersonalUse) {
        return mCtHost->findCurrentUserCron();
    } else {
        return mCtHost->findSystemCron();
    }
}

void KCMCron::refreshCron()
{
    CTCron *ctCron = currentCron();

    mTasksModel->refresh(ctCron);
    mVariablesModel->refresh(ctCron);
}

void KCMCron::refreshPages()
{
    while (this->depth() > 1) {
        this->pop();
    }
}

TasksModel *KCMCron::tasksModel() const noexcept
{
    return mTasksModel;
}

VariablesModel *KCMCron::variablesModel() const noexcept
{
    return mVariablesModel;
}

bool KCMCron::isPersonalUse() const noexcept
{
    return mIsPersonalUse;
}

void KCMCron::setIsPersonalUse(bool flag)
{
    if (flag != mIsPersonalUse) {
        mIsPersonalUse = flag;
        refreshCron();
        Q_EMIT isPersonalUseChanged();
    }
}

QStringList KCMCron::userList() const noexcept
{
    return mUserList;
}

#include "kcmCron.moc"

#include "moc_kcmCron.cpp"
