/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctSaveStatus.h"

CTSaveStatus::CTSaveStatus()
{
    mErrorStatus = false;
}

CTSaveStatus::CTSaveStatus(const QString &errorMessage, const QString &detailErrorMessage)
{
    mErrorStatus = true;
    mError = errorMessage;
    mDetailError = detailErrorMessage;
}

QString CTSaveStatus::errorMessage() const
{
    return mError;
}

QString CTSaveStatus::detailErrorMessage() const
{
    return mDetailError;
}

bool CTSaveStatus::isError() const
{
    return mErrorStatus;
}
