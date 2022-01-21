/*
    CT Environment Variable Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctvariable.h"

#include <KLocalizedString>
#include <QRegularExpression>

#include "ctHelper.h"

CTVariable::CTVariable(const QString &tokenString, const QString &_comment, const QString &_userLogin)
{
    QString tokStr = tokenString;

    if (tokStr.mid(0, 2) == QLatin1String("#\\")) {
        tokStr = tokStr.mid(2, tokStr.length() - 2);
        enabled = false;
    } else {
        enabled = true;
    }

    const int spacepos = tokStr.indexOf(QRegularExpression(QLatin1String("[ =]")));
    variable = tokStr.mid(0, spacepos);

    value = tokStr.mid(spacepos + 1, tokStr.length() - spacepos - 1);
    comment = _comment;

    userLogin = _userLogin;

    mInitialVariable = variable;
    mInitialValue = value;
    mInitialComment = comment;
    mInitialUserLogin = userLogin;
    mInitialEnabled = enabled;
}

CTVariable::CTVariable(const CTVariable &source)
    : variable(source.variable)
    , value(source.value)
    , comment(source.comment)
    , userLogin(source.userLogin)
    , enabled(source.enabled)
    , mInitialVariable(QLatin1String(""))
    , mInitialValue(QLatin1String(""))
    , mInitialComment(QLatin1String(""))
    , mInitialUserLogin(QLatin1String(""))
    , mInitialEnabled(true)
{
}

CTVariable &CTVariable::operator=(const CTVariable &source)
{
    if (this == &source) {
        return *this;
    }

    variable = source.variable;
    value = source.value;
    comment = source.comment;
    userLogin = source.userLogin;
    enabled = source.enabled;

    mInitialVariable = QLatin1String("");
    mInitialValue = QLatin1String("");
    mInitialComment = QLatin1String("");
    mInitialUserLogin = QLatin1String("");
    mInitialEnabled = true;
    return *this;
}

QString CTVariable::exportVariable()
{
    QString exportVariable = CTHelper::exportComment(comment);

    if (!enabled) {
        exportVariable += QLatin1String("#\\");
    }

    exportVariable += variable + QLatin1String("=") + value + QLatin1String("\n");

    return exportVariable;
}

void CTVariable::apply()
{
    mInitialVariable = variable;
    mInitialValue = value;
    mInitialComment = comment;
    mInitialUserLogin = userLogin;

    mInitialEnabled = enabled;
}

void CTVariable::cancel()
{
    variable = mInitialVariable;
    value = mInitialValue;
    comment = mInitialComment;
    userLogin = mInitialUserLogin;
    enabled = mInitialEnabled;
}

bool CTVariable::dirty() const
{
    return (variable != mInitialVariable) || (value != mInitialValue) || (comment != mInitialComment) || (userLogin != mInitialUserLogin)
        || (enabled != mInitialEnabled);
}

QIcon CTVariable::variableIcon() const
{
    if (variable == QLatin1String("MAILTO")) {
        return QIcon::fromTheme(QLatin1String("mail-message"));
    } else if (variable == QLatin1String("SHELL")) {
        return QIcon::fromTheme(QLatin1String("utilities-terminal"));
    } else if (variable == QLatin1String("HOME")) {
        return QIcon::fromTheme(QLatin1String("go-home"));
    } else if (variable == QLatin1String("PATH")) {
        return QIcon::fromTheme(QLatin1String("folder"));
    } else if (variable == QLatin1String("LD_CONFIG_PATH")) {
        return QIcon::fromTheme(QLatin1String("application-x-sharedlib"));
    }

    return QIcon::fromTheme(QLatin1String("text-plain"));
}

QString CTVariable::information() const
{
    if (variable == QLatin1String("HOME")) {
        return i18n("Override default home folder.");
    } else if (variable == QLatin1String("MAILTO")) {
        return i18n("Email output to specified account.");
    } else if (variable == QLatin1String("SHELL")) {
        return i18n("Override default shell.");
    } else if (variable == QLatin1String("PATH")) {
        return i18n("Folders to search for program files.");
    } else if (variable == QLatin1String("LD_CONFIG_PATH")) {
        return i18n("Dynamic libraries location.");
    }

    return i18n("Local Variable");
}
