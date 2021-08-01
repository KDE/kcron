/*
    KT icons.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QString>
#include <QWidget>

class QComboBox;
class QTextEdit;

class CrontabWidget;

class KCronHelper
{
public:
    static void initUserCombo(QComboBox *userCombo, CrontabWidget *crontabWidget, const QString &selectedUserLogin);

    static QTextEdit *createCommentEdit(QWidget *parent);
};

