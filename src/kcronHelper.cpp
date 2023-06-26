/*
    KT icons implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcronHelper.h"

#include <QComboBox>
#include <QFontMetrics>
#include <QTextEdit>

#include "ctcron.h"
#include "cthost.h"

#include "crontabWidget.h"

void KCronHelper::initUserCombo(QComboBox *userCombo, CrontabWidget *crontabWidget, const QString &selectedUserLogin)
{
    // This only applies to the System Crontab.
    // Populate the "Run as:" combobox with the login names of all current crons,
    // selecting the current user (root) as the default selection.
    int userComboIndex = 0;

    QStringList users;
    int selectedIndex = 0;
    const auto crons = crontabWidget->ctHost()->mCrons;
    for (CTCron *ctCron : crons) {
        users.append(ctCron->userLogin());

        if (ctCron->userLogin() == selectedUserLogin) {
            selectedIndex = userComboIndex;
        }

        userComboIndex++;
    }

    users.sort();

    userCombo->addItems(users);
    userCombo->setCurrentIndex(selectedIndex);
}

QTextEdit *KCronHelper::createCommentEdit(QWidget *parent)
{
    auto edit = new QTextEdit(parent);
    edit->setAcceptRichText(false);
    edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    edit->setTabChangesFocus(true);

    const QFontMetrics fontMetrics(edit->currentFont());
    edit->setMaximumHeight(fontMetrics.lineSpacing() * 3); // TODO Choose a smarter value

    return edit;
}
