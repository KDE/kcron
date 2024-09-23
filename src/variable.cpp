/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 2024 Evgeny Chesnokov <echesnokov@astralinux.ru>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "variable.h"

#include "ctvariable.h"

Variable::Variable(CTVariable *variable, QObject *parent) noexcept
    : QObject(parent)
    , mVariable(variable)
{
    Q_ASSERT(variable);
}

Variable::~Variable()
{
}

QString Variable::name() const noexcept
{
    return mVariable->variable;
}

bool Variable::setName(const QString &name) noexcept
{
    if (name != mVariable->variable) {
        mVariable->variable = name;
        Q_EMIT nameChanged();
        return true;
    }

    return false;
}

QString Variable::value() const noexcept
{
    return mVariable->value;
}

bool Variable::setValue(const QString &value) noexcept
{
    if (value != mVariable->value) {
        mVariable->value = value;
        Q_EMIT valueChanged();
        return true;
    }

    return false;
}

QString Variable::comment() const noexcept
{
    return mVariable->comment;
}

bool Variable::setComment(const QString &comment) noexcept
{
    if (comment != mVariable->comment) {
        mVariable->comment = comment;
        Q_EMIT commentChanged();
        return true;
    }

    return false;
}

QString Variable::userLogin() const noexcept
{
    return mVariable->userLogin;
}

bool Variable::setUserLogin(const QString &userLogin) noexcept
{
    if (userLogin != mVariable->userLogin) {
        mVariable->userLogin = userLogin;
        Q_EMIT userLoginChanged();
        return true;
    }

    return false;
}

bool Variable::enabled() const noexcept
{
    return mVariable->enabled;
}

bool Variable::setEnabled(bool enabled) noexcept
{
    if (enabled != mVariable->enabled) {
        mVariable->enabled = enabled;
        Q_EMIT enabledChanged();
        return true;
    }

    return false;
}

QString Variable::icon() const noexcept
{
    return mVariable->variableIcon().name();
}

QString Variable::information() const noexcept
{
    return mVariable->information();
}

QString Variable::exportVariable()
{
    return mVariable->exportVariable();
}

CTVariable *Variable::variable() const noexcept
{
    return mVariable;
}

void Variable::updateVariable(const CTVariable &variable) noexcept
{
    *mVariable = variable;
}

void Variable::updateVariable(CTVariable *variable) noexcept
{
    *mVariable = *variable;
}

void Variable::apply()
{
    mVariable->apply();
    Q_EMIT applyed();
}

void Variable::cancel()
{
    mVariable->cancel();
    Q_EMIT canceled();
}

bool Variable::hasChanges() const
{
    return mVariable->dirty();
}

#include "variable.moc"

#include "moc_variable.cpp"
