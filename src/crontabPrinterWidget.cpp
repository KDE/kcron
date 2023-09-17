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
    setWindowTitle(i18nc("@title:window", "Cron Options"));

    auto main_ = new QVBoxLayout(this);

    mChkPrintCrontab = new QCheckBox(i18n("Print cron&tab"), this);
    mChkPrintCrontab->setObjectName(QStringLiteral("chkPrintCrontab"));
    main_->addWidget(mChkPrintCrontab);

    mChkPrintAllUsers = new QCheckBox(i18n("Print &all users"), this);
    mChkPrintAllUsers->setObjectName(QStringLiteral("chkPrintAllUsers"));
    main_->addWidget(mChkPrintAllUsers);

    if (!root) {
        mChkPrintAllUsers->setChecked(false);
        mChkPrintAllUsers->setEnabled(false);
    }
}

CrontabPrinterWidget::~CrontabPrinterWidget()
{
}

bool CrontabPrinterWidget::printCrontab()
{
    return mChkPrintCrontab->isChecked();
}

void CrontabPrinterWidget::setPrintCrontab(bool setStatus)
{
    mChkPrintCrontab->setChecked(setStatus);
}

bool CrontabPrinterWidget::printAllUsers()
{
    return mChkPrintAllUsers->isChecked();
}

void CrontabPrinterWidget::setPrintAllUsers(bool setStatus)
{
    mChkPrintAllUsers->setChecked(setStatus);
}

#include "moc_crontabPrinterWidget.cpp"
