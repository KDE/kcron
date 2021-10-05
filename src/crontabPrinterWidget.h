/*
    --------------------------------------------------------------------
    Print Options Dialog
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Robert Berry <rjmber@ntlwolrd.com>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

class QCheckBox;

/**
 * Give the user the option to print the crontab file.
 * If the user is root ask if they want to print all the users.
 */

class CrontabPrinterWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Constructs the dialog. If root is true, the "print all users" is not disabled.
     */
    explicit CrontabPrinterWidget(bool root = false);

    ~CrontabPrinterWidget() override;

    bool printCrontab();
    void setPrintCrontab(bool setStatus);

    bool printAllUsers();
    void setPrintAllUsers(bool setStatus);

private:
    QCheckBox *mChkPrintCrontab = nullptr;
    QCheckBox *mChkPrintAllUsers = nullptr;
};

