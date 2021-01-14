/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CT_SAVE_STATUS
#define CT_SAVE_STATUS

#include <QString>

class CTSaveStatus
{
public:

    CTSaveStatus()
    {
        this->mErrorStatus = false;
    }

    CTSaveStatus(const QString &errorMessage, const QString &detailErrorMessage)
    {
        this->mErrorStatus = true;
        this->mError = errorMessage;
        this->mDetailError = detailErrorMessage;
    }

    QString errorMessage() const
    {
        return mError;
    }

    QString detailErrorMessage() const
    {
        return mDetailError;
    }

    bool isError() const
    {
        return mErrorStatus;
    }

private:
    bool mErrorStatus;

    QString mError;

    QString mDetailError;
};

#endif // CT_SAVE_STATUS
