/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QString>

class CTInitializationError
{
public:
    QString errorMessage() const;

    void setErrorMessage(const QString &errorMessage);

    bool hasErrorMessage() const;

private:
    QString mError;
};
