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
    QString source = args[QLatin1String("source")].toString();
    QString destination = args[QLatin1String("target")].toString();
    if (!QFile::remove(destination))
        qCWarning(KCM_CRON_HELPER_LOG) << "can't remove file, it doesn't exist";
    if (!QFile::copy(source, destination))
        qCWarning(KCM_CRON_HELPER_LOG) << "can't write into the system file, something went wrong";
    return ActionReply::SuccessReply();
}

KAUTH_HELPER_MAIN("local.kcron.crontab", KcronHelper)
