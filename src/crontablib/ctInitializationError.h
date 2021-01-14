/*
    CT Hour Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CT_INITIALIZATION_ERROR
#define CT_INITIALIZATION_ERROR

#include <QString>

class CTInitializationError
{
public:

    QString errorMessage() const
    {
        return mError;
    }

    void setErrorMessage(const QString &errorMessage)
    {
        this->mError = errorMessage;
    }

    bool hasErrorMessage()
    {
        if (mError.isEmpty()) {
            return false;
        }

        return true;
    }

private:
    QString mError;
};

#endif // CT_INITIALIZATION_ERROR
