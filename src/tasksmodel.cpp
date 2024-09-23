/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tasksmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>
#include <QProcess>
#include <QSortFilterProxyModel>

#include <KLocalizedString>

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"
#include "task.h"

#include "kcm_cron_debug.h"

TasksModel::TasksModel(QObject *parent) noexcept
    : GenericModel(parent)
    , mTmpTask(new Task(new CTTask(QStringLiteral(""), QStringLiteral(""), QStringLiteral("")), this))
{
    mProxyModel->sort(0, Qt::AscendingOrder);
}

TasksModel::~TasksModel()
{
    this->clear();
}

void TasksModel::run()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    QModelIndex currentIndex = mProxyModel->mapToSource(mSelectionModel->currentIndex());
    Task *task = mTasks.at(currentIndex.row());

    const QString echoMessage = i18nc("Do not use any quote characters (') in this string", "End of script execution. Type Enter or Ctrl+C to exit.");

    const auto variables = mCtCron->variables();

    QStringList commandList;
    commandList.reserve(variables.count() + 5);
    for (const auto &variable : variables) {
        if (variable->enabled) {
            commandList << QStringLiteral("export %1=%2").arg(variable->variable, variable->value);
        }
    }

    commandList << task->command();
    commandList << QStringLiteral("echo '-------------------------------------------------------------------------'");
    commandList << QStringLiteral("echo %1").arg(echoMessage);
    commandList << QStringLiteral("echo '-------------------------------------------------------------------------'");
    commandList << QStringLiteral("read");

    QStringList parameters;
    parameters << QStringLiteral("-e") << QStringLiteral("bash") << QStringLiteral("-c");
    parameters << commandList.join(QLatin1Char(';'));

    QProcess::startDetached(QStringLiteral("konsole"), parameters);
}

Task *TasksModel::create()
{
    // reset values of task from prev use
    CTTask ctTask = CTTask(QStringLiteral(""), QStringLiteral(""), mCtCron->userLogin(), mCtCron->isSystemCron());
    mTmpTask->updateTask(ctTask);
    return mTmpTask;
}

void TasksModel::applyCreate()
{
    Task *task = new Task(new CTTask(QStringLiteral(""), QStringLiteral(""), mCtCron->userLogin(), mCtCron->isSystemCron()), this);
    task->updateTask(mTmpTask->task());

    this->add(task);
    Q_EMIT addTask(task);
    Q_EMIT enabledStateChanged();
}

Task *TasksModel::modify()
{
    if (!mSelectionModel->hasSelection()) {
        qCWarning(KCM_CRON_LOG) << "There are no elements selected in the model";
        return this->create();
    }

    QModelIndex currentIndex = mProxyModel->mapToSource(mSelectionModel->currentIndex());
    Task *task = mTasks.at(currentIndex.row());

    mTmpTask->updateTask(task->task());
    return mTmpTask;
}

void TasksModel::applyModify()
{
    if (!mSelectionModel->hasSelection()) {
        qCWarning(KCM_CRON_LOG) << "There are no elements selected in the model, nothing can be updated";
        return;
    }

    QModelIndex currentIndex = mProxyModel->mapToSource(mSelectionModel->currentIndex());
    Task *task = mTasks.at(currentIndex.row());
    task->updateTask(mTmpTask->task());
    task->apply();
}

int TasksModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mTasks.count();
}

QVariant TasksModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid)) {
        return QVariant{};
    }

    Task *task = mTasks.at(index.row());

    switch (role) {
    case Roles::CommandRole:
        return QVariant::fromValue(task->command());
    case Roles::UserLoginRole:
        return QVariant::fromValue(task->userLogin());
    case Roles::EnabledRole:
        return QVariant::fromValue(task->enabled());
    case Roles::CommentRole:
        return QVariant::fromValue(task->comment());
    case Roles::CronFormatRole:
        return QVariant::fromValue(task->schedulingCronFormat());
    case Roles::DescriptionRole:
        return QVariant::fromValue(task->describe());
    default:
        Q_ASSERT(false);
    }

    return QVariant{};
}

bool TasksModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid)) {
        return false;
    }

    if (role != Roles::EnabledRole) {
        return false;
    }

    if (!value.canConvert(QMetaType(QMetaType::Bool))) {
        return false;
    }

    Task *task = mTasks.at(index.row());
    if (task->setEnabled(value.toBool())) {
        task->apply();
        return true;
    }

    return false;
}

QHash<int, QByteArray> TasksModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {CommandRole, "command"},
        {CommentRole, "comment"},
        {UserLoginRole, "userLogin"},
        {EnabledRole, "enabled"},
        {CronFormatRole, "cronFormat"},
        {DescriptionRole, "description"},
    };
    return roles;
}

void TasksModel::removeSelected()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    for (int i = mTasks.count() - 1; i >= 0; --i) {
        QModelIndex index = mProxyModel->mapFromSource(this->index(i, 0));
        if (mSelectionModel->isSelected(index)) {
            beginRemoveRows(QModelIndex(), i, i);

            Task *task = mTasks.at(i);
            Q_EMIT removeTask(task);
            this->remove(task);

            endRemoveRows();
        }
    }

    mSelectionModel->clear();

    Q_EMIT enabledStateChanged();
}

void TasksModel::copy()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    QString copyString;
    for (const auto &selectedIndex : mSelectionModel->selectedIndexes()) {
        QModelIndex index = mProxyModel->mapToSource(selectedIndex);
        copyString.append(mTasks.at(index.row())->exportTask());
        copyString.append(QLatin1Char('\n'));
    }

    if (!copyString.isEmpty()) {
        QApplication::clipboard()->setText(copyString);
    }
}

void TasksModel::cut()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    this->copy();
    this->removeSelected();
}

void TasksModel::paste()
{
    QString clipboard = QApplication::clipboard()->text();
    if (clipboard.isEmpty()) {
        return;
    }

    auto isTask = [](const QString &line) -> bool {
        static QRegularExpression whiteSpace = QRegularExpression(QLatin1String("[ \t]"));

        int firstWhiteSpace = line.indexOf(whiteSpace);
        int firstEquals = line.indexOf(QLatin1String("="));

        return firstEquals <= 0 || (firstWhiteSpace != -1 && firstWhiteSpace <= firstEquals);
    };

    auto isComment = [](const QString &line) -> bool {
        return line.indexOf(QLatin1String("#")) == 0 && line.indexOf(QLatin1String("\\")) != 1;
    };

    auto lines = clipboard.split(QLatin1Char('\n'));
    QString comment;
    for (QString line : lines) {
        // skip empty lines, empty lines are space between commented cron expressions
        if (line.isEmpty()) {
            comment.clear();
            continue;
        }

        // search for comments "#" but not disabled tasks "#\"
        // It's always loading comments user added by crontab -e command
        // or user added by Kcron
        if (isComment(line)) {
            line = line.mid(1, line.length() - 1);
            if (comment.isEmpty()) {
                comment = line.trimmed();
            } else {
                comment += QLatin1Char('\n') + line.trimmed();
            }
            continue;
        }

        if (isTask(line)) {
            Task *task = new Task(new CTTask(line, comment, mCtCron->userLogin(), mCtCron->isSystemCron()), this);
            this->add(task);
            Q_EMIT addTask(task);
            Q_EMIT enabledStateChanged();
            comment.clear();
        }
    }
}

bool TasksModel::needUserColumn()
{
    return mCtCron && mCtCron->isMultiUserCron();
}

Qt::CheckState TasksModel::enabledState()
{
    int count = enabledCount();

    if (count == 0) {
        return Qt::CheckState::Unchecked;
    }

    if (count == mTasks.length()) {
        return Qt::CheckState::Checked;
    }

    return Qt::CheckState::PartiallyChecked;
}

void TasksModel::setEnabledState(Qt::CheckState state)
{
    bool status = state == Qt::CheckState::Checked;

    for (auto &task : mTasks) {
        task->setEnabled(status);
        task->apply();
    }
}

void TasksModel::refresh(CTCron *ctCron)
{
    qCDebug(KCM_CRON_LOG) << "Refreshing tasks model";

    mCtCron = ctCron;
    Q_EMIT needUserColumnChanged();

    this->clear();

    for (CTTask *ctTask : mCtCron->tasks()) {
        Task *task = new Task(ctTask, this);
        this->add(task);
    }
}

void TasksModel::clear()
{
    if (mTasks.isEmpty()) {
        return;
    }

    qCDebug(KCM_CRON_LOG) << "Clearing tasks model";

    qDeleteAll(mTasks.begin(), mTasks.end());
    mTasks.clear();
    mSelectionModel->clear();

    beginResetModel();
    endResetModel();
}

int TasksModel::enabledCount()
{
    int ret = 0;
    for (const auto &task : mTasks) {
        if (task->enabled()) {
            ret++;
        }
    }

    return ret;
}

void TasksModel::add(Task *task)
{
    beginInsertRows(QModelIndex(), mTasks.count(), mTasks.count());
    mTasks.push_back(task);
    endInsertRows();

    QModelIndex modelIndex = index(mTasks.count() - 1, 0);
    QObject::connect(task, &Task::applyed, this, [this, modelIndex, task]() -> void {
        Q_EMIT dataChanged(modelIndex, modelIndex, {});
        Q_EMIT modifyTask(task);
        Q_EMIT enabledStateChanged();
    });
}

void TasksModel::remove(Task *task)
{
    mTasks.removeAll(task);
    delete task;
}

#include "tasksmodel.moc"

#include "moc_tasksmodel.cpp"
