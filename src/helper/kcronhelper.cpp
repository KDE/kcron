/* ========================================================================
 *    Copyright (C) 2015-2021 Blaze <blaze@vivaldi.net>
 *
 *    This file is part of Zeit.
 *
 *    Zeit is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    Zeit is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with Zeit.  If not, see <http://www.gnu.org/licenses/>.
 * ======================================================================== *
 *
 *    This file was modified to fit into the project Kcron.
 *    The same license terms apply.
 *
 * ======================================================================== */

#include <QFile>

#include "kcm_cron_helper_debug.h"

#include "kcronhelper.h"

ActionReply KcronHelper::save(const QVariantMap &args)
{
    qCDebug(KCM_CRON_HELPER_LOG) << "running actions";

    QByteArray newCronData;
    {
        const QString source = args[QLatin1String("source")].toString();
        QFile sourceFile(source);
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            qCWarning(KCM_CRON_HELPER_LOG) << "can't open source file for reading" << source << sourceFile.errorString();
            ActionReply reply = ActionReply::HelperErrorReply();
            reply.setErrorDescription(sourceFile.errorString());
            return reply;
        }

        newCronData = sourceFile.readAll();
    }

    {
        const QString destination = QStringLiteral("/etc/crontab");
        QFile destinationFile(destination);
        if (!destinationFile.open(QIODevice::WriteOnly)) {
            ActionReply reply = ActionReply::HelperErrorReply();
            qCWarning(KCM_CRON_HELPER_LOG) << "can't open destination file for writing" << destinationFile.errorString();
            reply.setErrorDescription(destinationFile.errorString());
            return reply;
        }

        if (destinationFile.write(newCronData) < 0) {
            ActionReply reply = ActionReply::HelperErrorReply();
            qCWarning(KCM_CRON_HELPER_LOG) << "writing to destination file failed" << destinationFile.errorString();
            reply.setErrorDescription(destinationFile.errorString());
        }
    }

    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("local.kcron.crontab", KcronHelper)
