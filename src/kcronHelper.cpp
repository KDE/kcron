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
#include "cttask.h"

#include "crontabWidget.h"

void KCronHelper::initUserCombo(QComboBox *userCombo, CrontabWidget *crontabWidget, const QString &selectedUserLogin)
{
    int userComboIndex = 0;

    QStringList users;
    int selectedIndex = 0;
    const auto crons = crontabWidget->ctHost()->mCrons;
    for (CTCron *ctCron : crons) {
        if (ctCron->isSystemCron()) {
            continue;
        }

        users.append(ctCron->userLogin());

        // Select the actual user
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
