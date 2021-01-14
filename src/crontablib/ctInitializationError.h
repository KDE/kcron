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
        return error;
    }

    void setErrorMessage(const QString &errorMessage)
    {
        this->error = errorMessage;
    }

    bool hasErrorMessage()
    {
        if (error.isEmpty() == true) {
            return false;
        }

        return true;
    }

private:
    QString error;
};

#endif // CT_INITIALIZATION_ERROR
