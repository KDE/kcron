/*
    CT Environment Variable Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QIcon>
#include <QString>

/**
 * An environment variable (encapsulation of crontab environment variable
 * entry).  Encapsulates parsing and tokenization.
 */
class CTVariable
{
public:
    /**
     * Constructs environment variable from crontab format string.
     */
    explicit CTVariable(const QString &tokenString, const QString &_comment, const QString &_userLogin);

    /**
     * Copy constructor.
     */
    CTVariable(const CTVariable &source);

    /**
     * Assignment operator.
     */
    CTVariable &operator=(const CTVariable &source);

    /**
     * Tokenizes environment variable to crontab format.
     */
    QString exportVariable();

    /**
     * Mark changes as applied.
     */
    void apply();

    /**
     * Cancel changes.
     */
    void cancel();

    /**
     * Indicates whether or not the environment variable has been modified.
     */
    bool dirty() const;

    QIcon variableIcon() const;

    QString information() const;

    QString variable;
    QString value;
    QString comment;
    QString userLogin;

    bool enabled;

private:
    QString mInitialVariable;
    QString mInitialValue;
    QString mInitialComment;
    QString mInitialUserLogin;
    bool mInitialEnabled;
};

