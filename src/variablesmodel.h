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
class Variable;

class VariablesModel final : public GenericModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        ValueRole,
        CommentRole,
        UserLoginRole,
        EnabledRole,
        IconRole,
        InformationRole,
    };
    Q_ENUM(Roles)

public:
    explicit VariablesModel(QObject *parent = nullptr) noexcept;
    ~VariablesModel() override;

    Q_INVOKABLE Variable *create();
    Q_INVOKABLE void applyCreate();

    Q_INVOKABLE Variable *modify();
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
    void addVariable(Variable *variable);
    void modifyVariable(Variable *variable);
    void removeVariable(Variable *variable);

private:
    void clear() override;
    int enabledCount() override;

    void add(Variable *variable);
    void remove(Variable *variable);

private:
    QList<Variable *> mVariables;

    Variable *const mTmpVariable;
};
