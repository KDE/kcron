/*
    CT Task Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "cttask.h"

#include <KLocalizedString>

#include <QMimeDatabase>
#include <QRegularExpression>
#include <QUrl>

#include "ctHelper.h"

CTTask::CTTask(const QString &tokenString, const QString &_comment, const QString &_userLogin, bool _systemCrontab)
    : mSystemCrontab(_systemCrontab)
{
    QString tokStr = tokenString;
    if (tokStr.mid(0, 2) == QLatin1String("#\\")) {
        tokStr = tokStr.mid(2, tokStr.length() - 2);
        enabled = false;
    } else if (tokStr.mid(0, 1) == QLatin1String("#")) {
        tokStr = tokStr.mid(1, tokStr.length() - 1);
        enabled = false;
    } else {
        enabled = true;
    }

    // Skip over 'silence' if found... old option in vixie cron
    if (tokStr.mid(0, 1) == QLatin1String("-")) {
        tokStr = tokStr.mid(1, tokStr.length() - 1);
    }

    reboot = false;
    if (tokStr.mid(0, 1) == QLatin1String("@")) {
        if (tokStr.mid(1, 6) == QLatin1String("yearly")) {
            tokStr = QLatin1String("0 0 1 1 *") + tokStr.mid(7, tokStr.length() - 1);
        } else if (tokStr.mid(1, 8) == QLatin1String("annually")) {
            tokStr = QLatin1String("0 0 1 1 *") + tokStr.mid(9, tokStr.length() - 1);
        } else if (tokStr.mid(1, 7) == QLatin1String("monthly")) {
            tokStr = QLatin1String("0 0 1 * *") + tokStr.mid(8, tokStr.length() - 1);
        } else if (tokStr.mid(1, 6) == QLatin1String("weekly")) {
            tokStr = QLatin1String("0 0 * * 0") + tokStr.mid(7, tokStr.length() - 1);
        } else if (tokStr.mid(1, 5) == QLatin1String("daily")) {
            tokStr = QLatin1String("0 0 * * *") + tokStr.mid(6, tokStr.length() - 1);
        } else if (tokStr.mid(1, 6) == QLatin1String("hourly")) {
            tokStr = QLatin1String("0 * * * *") + tokStr.mid(7, tokStr.length() - 1);
        } else if (tokStr.mid(1, 6) == QLatin1String("reboot")) {
            tokStr = tokStr.mid(7, tokStr.length() - 1);
            reboot = true;
        }
    }

    int spacePos(tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]"))));
    // If reboot bypass initialize functions so no keys selected in modify task
    if (!reboot) {
        minute.initialize(tokStr.mid(0, spacePos));

        while (isSpaceAt(tokStr, spacePos + 1)) {
            spacePos++;
        }
        tokStr = tokStr.mid(spacePos + 1, tokStr.length() - 1);
        spacePos = tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]")));
        hour.initialize(tokStr.mid(0, spacePos));

        while (isSpaceAt(tokStr, spacePos + 1)) {
            spacePos++;
        }
        tokStr = tokStr.mid(spacePos + 1, tokStr.length() - 1);
        spacePos = tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]")));
        dayOfMonth.initialize(tokStr.mid(0, spacePos));

        while (isSpaceAt(tokStr, spacePos + 1)) {
            spacePos++;
        }
        tokStr = tokStr.mid(spacePos + 1, tokStr.length() - 1);
        spacePos = tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]")));
        month.initialize(tokStr.mid(0, spacePos));

        while (isSpaceAt(tokStr, spacePos + 1)) {
            spacePos++;
        }
        tokStr = tokStr.mid(spacePos + 1, tokStr.length() - 1);
        spacePos = tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]")));
        dayOfWeek.initialize(tokStr.mid(0, spacePos));
    }

    // Since it's a multiuser(system) task, the token contains the user login,
    // and the command, separated by a tab (\t).
    // The two need to subsequently be separated again.
    // E.g. "root\tmy_test_script.sh"
    if (mSystemCrontab) {
        while (isSpaceAt(tokStr, spacePos + 1)) {
            spacePos++;
        }
        tokStr = tokStr.mid(spacePos + 1, tokStr.length() - 1);
        spacePos = tokStr.indexOf(QRegularExpression(QLatin1String("[ \t]")));
        userLogin = tokStr.mid(0, spacePos);
    } else {
        userLogin = _userLogin;
    }
    command = tokStr.mid(spacePos + 1, tokStr.length() - 1);
    // remove leading whitespace
    while (command.indexOf(QRegularExpression(QLatin1String("[ \t]"))) == 0) {
        command = command.mid(1, command.length() - 1);
    }
    comment = _comment;

    mInitialUserLogin = userLogin;
    mInitialCommand = command;
    mInitialComment = comment;
    mInitialEnabled = enabled;
    mInitialReboot = reboot;
}

CTTask::CTTask(const CTTask &source)
    : month(source.month)
    , dayOfMonth(source.dayOfMonth)
    , dayOfWeek(source.dayOfWeek)
    , hour(source.hour)
    , minute(source.minute)
    , userLogin(source.userLogin)
    , command(source.command)
    , comment(source.comment)
    , enabled(source.enabled)
    , reboot(source.reboot)
    , mInitialUserLogin(QLatin1String(""))
    , mInitialCommand(QLatin1String(""))
    , mInitialComment(QLatin1String(""))
    , mInitialEnabled(true)
    , mInitialReboot(false)
{
}

CTTask &CTTask::operator=(const CTTask &source)
{
    if (this == &source) {
        return *this;
    }

    month = source.month;
    dayOfMonth = source.dayOfMonth;
    dayOfWeek = source.dayOfWeek;
    hour = source.hour;
    minute = source.minute;
    userLogin = source.userLogin;
    command = source.command;
    comment = source.comment;
    enabled = source.enabled;
    reboot = source.reboot;
    mInitialUserLogin = QLatin1String("");
    mInitialCommand = QLatin1String("");
    mInitialComment = QLatin1String("");
    mInitialEnabled = true;
    mInitialReboot = false;

    return *this;
}

QString CTTask::exportTask()
{
    QString exportTask;

    exportTask += CTHelper::exportComment(comment);

    if (!enabled) {
        exportTask += QLatin1String("#\\");
    }

    exportTask += schedulingCronFormat();
    exportTask += QLatin1String("\t");

    if (isSystemCrontab()) {
        exportTask += userLogin + QLatin1String("\t");
    }

    exportTask += command + QLatin1String("\n");

    return exportTask;
}

void CTTask::apply()
{
    month.apply();
    dayOfMonth.apply();
    dayOfWeek.apply();
    hour.apply();
    minute.apply();

    mInitialUserLogin = userLogin;
    mInitialCommand = command;
    mInitialComment = comment;
    mInitialEnabled = enabled;
    mInitialReboot = reboot;
}

void CTTask::cancel()
{
    month.cancel();
    dayOfMonth.cancel();
    dayOfWeek.cancel();
    hour.cancel();
    minute.cancel();

    userLogin = mInitialUserLogin;
    command = mInitialCommand;
    comment = mInitialComment;
    enabled = mInitialEnabled;
    reboot = mInitialReboot;
}

bool CTTask::dirty() const
{
    return month.isDirty() || dayOfMonth.isDirty() || dayOfWeek.isDirty() || hour.isDirty() || minute.isDirty() || (userLogin != mInitialUserLogin)
        || (command != mInitialCommand) || (comment != mInitialComment) || (enabled != mInitialEnabled) || (reboot != mInitialReboot);
}

QString CTTask::schedulingCronFormat() const
{
    if (reboot) {
        return QStringLiteral("@reboot");
    }

    QString scheduling = minute.exportUnit() + QLatin1Char(' ');
    scheduling += hour.exportUnit() + QLatin1Char(' ');
    scheduling += dayOfMonth.exportUnit() + QLatin1Char(' ');
    scheduling += month.exportUnit() + QLatin1Char(' ');
    scheduling += dayOfWeek.exportUnit();

    return scheduling;
}

/**
 * Of the whole program, this method is probably the trickiest.
 *
 * This method creates the natural language description, such as
 * "At 1:00am, every Sun".
 *
 * First, I declare some strings for holding what can be internationalized.
 * Note the tokens such as "MONTHS".  Translators should reuse these
 * tokens in their translations.  See README.translators for more
 * information.
 *
 * Second, I get the descriptions from the component parts such as
 * days of the month.
 *
 * Third, I get hour/minute time combinations.  Although a little bit
 * awkward, I use the tm struct and strftime from <time.h>.  This
 * way this code is portable across all Unixes.
 *
 * Fourth, I know that "every day of the week" and "every day of the
 * month" simply makes "every day".
 *
 * Fifth and finally I do tag substitution to create the natural language
 * description.
 *
 */
QString CTTask::describe() const
{
    if (reboot) {
        return i18n("At system startup");
    }

    QString dateFormat = createDateFormat();

    QString timeFormat = createTimeFormat();

    return i18nc("1:Time Description, 2:Date Description", "%1, %2", timeFormat, dateFormat);
}

QString CTTask::describeDayOfWeek() const
{
    return i18nc("Every 'days of week'", "every %1", dayOfWeek.describe());
}

QString CTTask::describeDayOfMonth() const
{
    return i18nc("'Days of month' of 'Months'", "%1 of %2", dayOfMonth.describe(), month.describe());
}

QString CTTask::createDateFormat() const
{
    /*
     * "* * *" means truly every day.
     * Note: Languages may use different phrases to indicate
     * every day of month versus every day of week.
     */
    QString dateFormat;
    if ((dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM) && (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM)) {
        dateFormat = i18n("every day ");
    }
    // Day of month not specified, so use day of week.
    else if (dayOfMonth.enabledCount() == CTDayOfMonth::MAXIMUM) {
        dateFormat = describeDayOfWeek();
    }
    // Day of week not specified, so use day of month.
    else if (dayOfWeek.enabledCount() == CTDayOfWeek::MAXIMUM) {
        dateFormat = describeDayOfMonth();
    } else {
        dateFormat = i18nc("1:Day of month, 2:Day of week", "%1 as well as %2", describeDayOfMonth(), describeDayOfWeek());
    }

    return dateFormat;
}

QString CTTask::describeDateAndHours() const
{
    // Create time description.
    int total = minute.enabledCount() * hour.enabledCount();

    QString timeDesc;
    int count = 0;

    for (int h = 0; h <= 23; h++) {
        if (hour.isEnabled(h)) {
            for (int m = 0; m <= 59; m++) {
                if (minute.isEnabled(m)) {
                    QString hourString;
                    if (h < 10) {
                        hourString = QLatin1String("0") + QString::number(h);
                    } else {
                        hourString = QString::number(h);
                    }

                    QString minuteString;
                    if (m < 10) {
                        minuteString = QLatin1String("0") + QString::number(m);
                    } else {
                        minuteString = QString::number(m);
                    }

                    QString tmpStr = i18nc("1:Hour, 2:Minute", "%1:%2", hourString, minuteString);

                    timeDesc += tmpStr;
                    count++;
                    switch (total - count) {
                    case 0:
                        break;
                    case 1:
                        if (total > 2) {
                            timeDesc += i18n(", and ");
                        } else {
                            timeDesc += i18n(" and ");
                        }
                        break;
                    default:
                        timeDesc += i18n(", ");
                    }
                }
            }
        }
    }

    return i18nc("Hour::Minute list", "At %1", timeDesc);
}

QString CTTask::createTimeFormat() const
{
    if (hour.isAllEnabled()) {
        int minutePeriod = minute.findPeriod();
        if (minutePeriod != 0) {
            return i18np("Every minute", "Every %1 minutes", minutePeriod);
        }
    }

    return describeDateAndHours();
}

bool CTTask::isSystemCrontab() const
{
    return mSystemCrontab;
}

void CTTask::setSystemCrontab(bool _systemCrontab)
{
    mSystemCrontab = _systemCrontab;
}

QIcon CTTask::commandIcon() const
{
    QUrl commandPath = QUrl::fromLocalFile(completeCommandPath());

    QMimeType mimeType = QMimeDatabase().mimeTypeForUrl(commandPath);
    if (mimeType.name() == QLatin1String("application/x-executable") || mimeType.name() == QLatin1String("application/octet-stream")) {
        return QIcon::fromTheme(commandPath.fileName(), QIcon::fromTheme(QLatin1String("system-run")));
    }

    return QIcon::fromTheme(mimeType.iconName());
}

QPair<QString, bool> CTTask::unQuoteCommand() const
{
    QString fullCommand = command;
    fullCommand = fullCommand.trimmed();

    const QStringList quotes{QStringLiteral("\""), QStringLiteral("'")};

    for (const QString &quote : quotes) {
        if (fullCommand.indexOf(quote) == 0) {
            int nextQuote = fullCommand.indexOf(quote, 1);
            if (nextQuote == -1) {
                return QPair<QString, bool>(QLatin1String(""), false);
            }

            return QPair<QString, bool>(fullCommand.mid(1, nextQuote - 1), true);
        }
    }

    return QPair<QString, bool>(fullCommand, false);
}

QString CTTask::decryptBinaryCommand(const QString &command) const
{
    QString fullCommand;

    bool found = false;
    for (int i = 0; i < command.length(); ++i) {
        if (command.at(i) == QLatin1Char(' ') && command.at(i - 1) != QLatin1Char('\\')) {
            fullCommand = command.left(i);
            found = true;
            break;
        }
    }

    if (!found) {
        fullCommand = command;
    }

    fullCommand.remove(QLatin1Char('\\'));

    return fullCommand;
}

QStringList CTTask::separatePathCommand(const QString &command, bool quoted) const
{
    QStringList pathCommand;

    if (command.at(0) == QLatin1Char('/')) {
        QString fullCommand;
        if (quoted) {
            fullCommand = command;
        } else {
            fullCommand = decryptBinaryCommand(command);
        }

        if (fullCommand.isEmpty()) {
            return QStringList();
        }

        QString path = fullCommand.section(QLatin1Char('/'), 0, -2);
        QString commandBinary = fullCommand.section(QLatin1Char('/'), -1);

        pathCommand << path << commandBinary;
    } else {
        QString fullCommand;
        if (quoted) {
            fullCommand = command;
        } else {
            fullCommand = decryptBinaryCommand(command);
        }

        // relying on $PATH
        pathCommand << QString() << fullCommand;
    }

    return pathCommand;
}

QString CTTask::completeCommandPath() const
{
    QPair<QString, bool> commandQuoted = unQuoteCommand();
    if (commandQuoted.first.isEmpty()) {
        return QLatin1String("");
    }

    QStringList pathCommand = separatePathCommand(commandQuoted.first, commandQuoted.second);
    if (pathCommand.isEmpty()) {
        return QLatin1String("");
    }

    return pathCommand.join(QLatin1String("/"));
}
