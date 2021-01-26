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
    QString errorMessage() const;

    void setErrorMessage(const QString &errorMessage);

    bool hasErrorMessage() const;

private:
    QString mError;
};

#endif // CT_INITIALIZATION_ERROR
