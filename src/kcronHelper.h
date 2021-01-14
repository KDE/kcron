/*
    KT icons.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KCRON_HELPER_H
#define KCRON_HELPER_H

#include <QPixmap>
#include <QString>

class QComboBox;
class QTextEdit;

class CrontabWidget;

class KCronHelper
{
public:
    static void initUserCombo(QComboBox *userCombo, CrontabWidget *crontabWidget, const QString &selectedUserLogin);

    static QTextEdit *createCommentEdit(QWidget *parent);
};

#endif // KCRON_HELPER_H
