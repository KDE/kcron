/*
    --------------------------------------------------------------------
    Print Options Dialog
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Robert Berry <rjmber@ntlwolrd.com>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "crontabPrinterWidget.h"

#include <QCheckBox>
#include <QVBoxLayout>

#include <KLocalizedString>

CrontabPrinterWidget::CrontabPrinterWidget(bool root)
    : QWidget()
{
    setWindowTitle(i18n("Cron Options"));

    QVBoxLayout *main_ = new QVBoxLayout(this);

    chkPrintCrontab = new QCheckBox(i18n("Print cron&tab"), this);
    chkPrintCrontab->setObjectName(QStringLiteral("chkPrintCrontab"));
    main_->addWidget(chkPrintCrontab);

    chkPrintAllUsers = new QCheckBox(i18n("Print &all users"), this);
    chkPrintAllUsers->setObjectName(QStringLiteral("chkPrintAllUsers"));
    main_->addWidget(chkPrintAllUsers);

    if (!root) {
        chkPrintAllUsers->setChecked(false);
        chkPrintAllUsers->setEnabled(false);
    }
}

CrontabPrinterWidget::~CrontabPrinterWidget()
{
}

bool CrontabPrinterWidget::printCrontab()
{
    return chkPrintCrontab->isChecked();
}

void CrontabPrinterWidget::setPrintCrontab(bool setStatus)
{
    chkPrintCrontab->setChecked(setStatus);
}

bool CrontabPrinterWidget::printAllUsers()
{
    return chkPrintAllUsers->isChecked();
}

void CrontabPrinterWidget::setPrintAllUsers(bool setStatus)
{
    chkPrintAllUsers->setChecked(setStatus);
}
