/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "variablesmodel.h"

#include <QApplication>
#include <QClipboard>
#include <QItemSelectionModel>
#include <QSortFilterProxyModel>

#include "ctcron.h"
#include "ctvariable.h"
#include "variable.h"

#include "kcm_cron_debug.h"

VariablesModel::VariablesModel(QObject *parent) noexcept
    : GenericModel(parent)
    , mTmpVariable(new Variable(new CTVariable(QStringLiteral(""), QStringLiteral(""), QStringLiteral("")), this))
{
    mProxyModel->setSortRole(Roles::NameRole);
    mProxyModel->sort(0, Qt::AscendingOrder);
}

VariablesModel::~VariablesModel()
{
    this->clear();
}

Variable *VariablesModel::create()
{
    // reset values of variable from prev use
    CTVariable ctVariable = CTVariable(QStringLiteral(""), QStringLiteral(""), mCtCron->userLogin());
    mTmpVariable->updateVariable(ctVariable);
    return mTmpVariable;
}

void VariablesModel::applyCreate()
{
    Variable *variable = new Variable(new CTVariable(QStringLiteral(""), QStringLiteral(""), mCtCron->userLogin()), this);
    variable->updateVariable(mTmpVariable->variable());

    this->add(variable);
    Q_EMIT addVariable(variable);
    Q_EMIT enabledStateChanged();
}

Variable *VariablesModel::modify()
{
    if (!mSelectionModel->hasSelection()) {
        qCWarning(KCM_CRON_LOG) << "There are no elements selected in the model";
        return this->create();
    }

    QModelIndex currentIndex = mProxyModel->mapToSource(mSelectionModel->currentIndex());
    Variable *variable = mVariables.at(currentIndex.row());

    mTmpVariable->updateVariable(variable->variable());
    return mTmpVariable;
}

void VariablesModel::applyModify()
{
    if (!mSelectionModel->hasSelection()) {
        qCWarning(KCM_CRON_LOG) << "There are no elements selected in the model, nothing can be updated";
        return;
    }

    QModelIndex currentIndex = mProxyModel->mapToSource(mSelectionModel->currentIndex());
    Variable *variable = mVariables.at(currentIndex.row());
    variable->updateVariable(mTmpVariable->variable());
    variable->apply();
}

int VariablesModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mVariables.count();
}

QVariant VariablesModel::data(const QModelIndex &index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::ParentIsInvalid | CheckIndexOption::IndexIsValid)) {
        return QVariant{};
    }

    Variable *variable = mVariables.at(index.row());

    switch (role) {
    case Roles::NameRole:
        return QVariant::fromValue(variable->name());
    case Roles::ValueRole:
        return QVariant::fromValue(variable->value());
    case Roles::CommentRole:
        return QVariant::fromValue(variable->comment());
    case Roles::UserLoginRole:
        return QVariant::fromValue(variable->userLogin());
    case Roles::EnabledRole:
        return QVariant::fromValue(variable->enabled());
    case Roles::IconRole:
        return QVariant::fromValue(variable->icon());
    case Roles::InformationRole:
        return QVariant::fromValue(variable->information());
    default:
        Q_ASSERT(false);
    }

    return QVariant{};
}

bool VariablesModel::setData(const QModelIndex &index, const QVariant &value, int role)
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

    Variable *variable = mVariables.at(index.row());
    if (variable->setEnabled(value.toBool())) {
        variable->apply();
        return true;
    }

    return false;
}

QHash<int, QByteArray> VariablesModel::roleNames() const
{
    static QHash<int, QByteArray> roles{
        {NameRole, "name"},
        {ValueRole, "value"},
        {CommentRole, "comment"},
        {UserLoginRole, "userLogin"},
        {EnabledRole, "enabled"},
        {IconRole, "icon"},
        {InformationRole, "information"},
    };
    return roles;
}

void VariablesModel::removeSelected()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    for (int i = mVariables.count() - 1; i >= 0; --i) {
        QModelIndex index = mProxyModel->mapFromSource(this->index(i, 0));
        if (mSelectionModel->isSelected(index)) {
            beginRemoveRows(QModelIndex(), i, i);

            Variable *variable = mVariables.at(i);
            Q_EMIT removeVariable(variable);
            this->remove(variable);

            endRemoveRows();
        }
    }

    mSelectionModel->clear();

    Q_EMIT enabledStateChanged();
}

void VariablesModel::copy()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    QString copyString;
    for (const auto &selectedIndex : mSelectionModel->selectedIndexes()) {
        QModelIndex index = mProxyModel->mapToSource(selectedIndex);
        copyString.append(mVariables.at(index.row())->exportVariable());
        copyString.append(QLatin1Char('\n'));
    }

    if (!copyString.isEmpty()) {
        QApplication::clipboard()->setText(copyString);
    }
}

void VariablesModel::cut()
{
    if (!mSelectionModel->hasSelection()) {
        return;
    }

    this->copy();
    this->removeSelected();
}

void VariablesModel::paste()
{
    QString clipboard = QApplication::clipboard()->text();
    if (clipboard.isEmpty()) {
        return;
    }

    auto isVariable = [](const QString &line) -> bool {
        static QRegularExpression whiteSpace = QRegularExpression(QLatin1String("[ \t]"));

        int firstWhiteSpace = line.indexOf(whiteSpace);
        int firstEquals = line.indexOf(QLatin1String("="));

        return (firstEquals > 0) && ((firstWhiteSpace == -1) || firstWhiteSpace > firstEquals);
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

        if (isVariable(line)) {
            Variable *variable = new Variable(new CTVariable(line, comment, mCtCron->userLogin()), this);
            this->add(variable);
            Q_EMIT addVariable(variable);
            Q_EMIT enabledStateChanged();
            comment.clear();
        }
    }
}

bool VariablesModel::needUserColumn()
{
    return mCtCron && mCtCron->isMultiUserCron() && !mCtCron->isSystemCron();
}

Qt::CheckState VariablesModel::enabledState()
{
    int count = enabledCount();

    if (count == 0) {
        return Qt::CheckState::Unchecked;
    }

    if (count == mVariables.length()) {
        return Qt::CheckState::Checked;
    }

    return Qt::CheckState::PartiallyChecked;
}

void VariablesModel::setEnabledState(Qt::CheckState state)
{
    bool status = state == Qt::CheckState::Checked;

    for (Variable *variable : mVariables) {
        variable->setEnabled(status);
        variable->apply();
    }
}

void VariablesModel::refresh(CTCron *ctCron)
{
    qCDebug(KCM_CRON_LOG) << "Refreshing variables model";

    mCtCron = ctCron;

    this->clear();

    for (CTVariable *ctVariable : mCtCron->variables()) {
        Variable *variable = new Variable(ctVariable, this);
        this->add(variable);
    }

    Q_EMIT needUserColumnChanged();
    Q_EMIT enabledStateChanged();
}

void VariablesModel::clear()
{
    if (mVariables.isEmpty()) {
        return;
    }

    qCDebug(KCM_CRON_LOG) << "Clearing variables model";

    qDeleteAll(mVariables.begin(), mVariables.end());
    mVariables.clear();
    mSelectionModel->clear();

    beginResetModel();
    endResetModel();
}

int VariablesModel::enabledCount()
{
    int ret = 0;
    for (const auto &variable : mVariables) {
        if (variable->enabled()) {
            ret++;
        }
    }

    return ret;
}

void VariablesModel::add(Variable *variable)
{
    beginInsertRows(QModelIndex(), mVariables.count(), mVariables.count());
    mVariables.push_back(variable);
    endInsertRows();

    QModelIndex modelIndex = index(mVariables.count() - 1, 0);
    QObject::connect(variable, &Variable::applyed, this, [this, modelIndex, variable]() -> void {
        Q_EMIT dataChanged(modelIndex, modelIndex, {});
        Q_EMIT modifyVariable(variable);
        Q_EMIT enabledStateChanged();
    });
}

void VariablesModel::remove(Variable *variable)
{
    mVariables.removeAll(variable);
    delete variable;
}

#include "variablesmodel.moc"

#include "moc_variablesmodel.cpp"
