/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctInitializationError.h"

QString CTInitializationError::errorMessage() const
{
    return mError;
}

void CTInitializationError::setErrorMessage(const QString &errorMessage)
{
    this->mError = errorMessage;
}

bool CTInitializationError::hasErrorMessage() const
{
    if (mError.isEmpty()) {
        return false;
    }

    return true;
}
