/*
    CT Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctHelper.h"

#include <QStringList>

#include <KLocalizedString>

QString CTHelper::exportComment(const QString &comment)
{
    QString exportComment;

    if (comment.isEmpty()) {
        QString noComment = i18n("No comment");
        exportComment += QLatin1String("#") + noComment + QLatin1String("\n");
        return exportComment;
    }

    QStringList lines = comment.split(QStringLiteral("\n"));
    foreach (const QString &line, lines) {
        exportComment += QLatin1String("#") + line + QLatin1String("\n");
    }

    return exportComment;
}
