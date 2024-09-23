/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "genericmodel.h"

#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include "kcm_cron_debug.h"

GenericModel::GenericModel(QObject *parent) noexcept
    : QAbstractListModel(parent)
    , mProxyModel(new QSortFilterProxyModel(this))
    , mSelectionModel(new QItemSelectionModel(this))
{
    mProxyModel->setSourceModel(this);
    mProxyModel->setSortRole(Qt::UserRole + 1);

    mSelectionModel->setModel(mProxyModel);
}

void GenericModel::selectAll()
{
    qCDebug(KCM_CRON_LOG) << "Trying to select all table elements";

    const int rowCount = mSelectionModel->model()->rowCount();
    if (rowCount < 1) {
        return;
    }

    for (int i = 0; i < rowCount; ++i) {
        QModelIndex index = mProxyModel->index(i, 0);
        mSelectionModel->select(index, QItemSelectionModel::SelectionFlag::Select);
    }
    mSelectionModel->setCurrentIndex(mProxyModel->mapToSource(mProxyModel->index(0, 0)), QItemSelectionModel::SelectionFlag::NoUpdate);
}

QSortFilterProxyModel *GenericModel::proxyModel() const
{
    return mProxyModel;
}

QItemSelectionModel *GenericModel::selectionModel() const
{
    return mSelectionModel;
}

#include "genericmodel.moc"

#include "moc_genericmodel.cpp"
