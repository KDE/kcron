/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QAbstractListModel>

class QSortFilterProxyModel;
class QItemSelectionModel;
class CTCron;

class GenericModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QItemSelectionModel *selectionModel READ selectionModel CONSTANT FINAL)
    Q_PROPERTY(QSortFilterProxyModel *proxyModel READ proxyModel CONSTANT FINAL)

    Q_PROPERTY(bool needUserColumn READ needUserColumn NOTIFY needUserColumnChanged FINAL)
    Q_PROPERTY(Qt::CheckState enabledState READ enabledState WRITE setEnabledState NOTIFY enabledStateChanged)

public:
    explicit GenericModel(QObject *parent) noexcept;
    ~GenericModel() override = default;

    Q_INVOKABLE void selectAll();

    Q_INVOKABLE virtual void removeSelected() = 0;

    Q_INVOKABLE virtual void copy() = 0;
    Q_INVOKABLE virtual void cut() = 0;
    Q_INVOKABLE virtual void paste() = 0;

    virtual bool needUserColumn() = 0;

    virtual Qt::CheckState enabledState() = 0;
    virtual void setEnabledState(Qt::CheckState state) = 0;

Q_SIGNALS:
    void needUserColumnChanged();
    void enabledStateChanged();

public Q_SLOTS:
    virtual void refresh(CTCron *ctCron) = 0;

protected:
    virtual void clear() = 0;
    virtual int enabledCount() = 0;

public:
    QSortFilterProxyModel *proxyModel() const;
    QItemSelectionModel *selectionModel() const;

protected:
    QSortFilterProxyModel *const mProxyModel;
    QItemSelectionModel *const mSelectionModel;

    CTCron *mCtCron = nullptr;
};
