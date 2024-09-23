/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "genericmodel.h"

class CTCron;
class CTTask;
class Task;

class TasksModel final : public GenericModel
{
    Q_OBJECT

public:
    enum Roles {
        CommandRole = Qt::UserRole + 1,
        UserLoginRole,
        EnabledRole,
        CommentRole,
        CronFormatRole,
        DescriptionRole,
    };
    Q_ENUM(Roles)

public:
    explicit TasksModel(QObject *parent) noexcept;
    ~TasksModel() override;

    Q_INVOKABLE void run();

    Q_INVOKABLE Task *create();
    Q_INVOKABLE void applyCreate();

    Q_INVOKABLE Task *modify();
    Q_INVOKABLE void applyModify();

    Q_INVOKABLE int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_INVOKABLE QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void removeSelected() override;

    Q_INVOKABLE void copy() override;
    Q_INVOKABLE void cut() override;
    Q_INVOKABLE void paste() override;

    bool needUserColumn() override;

    Qt::CheckState enabledState() override;
    void setEnabledState(Qt::CheckState state) override;

public Q_SLOTS:
    void refresh(CTCron *ctCron) override;

Q_SIGNALS:
    void addTask(Task *task);
    void modifyTask(Task *task);
    void removeTask(Task *task);

private:
    void clear() override;
    int enabledCount() override;

    void add(Task *task);
    void remove(Task *task);

private:
    QList<Task *> mTasks;

    Task *const mTmpTask;
};
