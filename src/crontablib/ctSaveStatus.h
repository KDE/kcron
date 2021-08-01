/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QString>

class CTSaveStatus
{
public:
    CTSaveStatus();

    CTSaveStatus(const QString &errorMessage, const QString &detailErrorMessage);

    QString errorMessage() const;

    QString detailErrorMessage() const;

    bool isError() const;

private:
    bool mErrorStatus;

    QString mError;

    QString mDetailError;
};

