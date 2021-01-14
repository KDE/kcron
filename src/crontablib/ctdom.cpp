/*
    CT Day of Month Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctdom.h"
#include <KLocalizedString>

QList<QString> CTDayOfMonth::shortName;

CTDayOfMonth::CTDayOfMonth(const QString &tokStr)
    : CTUnit(CTDayOfMonth::MINIMUM, CTDayOfMonth::MAXIMUM, tokStr)
{
}

QString CTDayOfMonth::describe() const
{
    initializeNames();
    return (enabledCount() == CTDayOfMonth::MAXIMUM) ? i18n("every day ") : CTUnit::genericDescribe(shortName);
}

QString CTDayOfMonth::getName(const int ndx)
{
    initializeNames();
    return shortName[ndx];
}

void CTDayOfMonth::initializeNames()
{
    if (shortName.isEmpty()) {
        shortName << QLatin1String("") << i18n("1st") << i18n("2nd") << i18n("3rd") << i18n("4th") << i18n("5th") << i18n("6th") << i18n("7th") << i18n("8th") << i18n("9th") << i18n("10th") << i18n("11th") << i18n("12th") << i18n("13th") << i18n("14th") << i18n("15th") << i18n("16th") <<
        i18n("17th")
                  << i18n("18th") << i18n("19th") << i18n("20th") << i18n("21st") << i18n("22nd") << i18n("23rd") << i18n("24th") << i18n("25th") << i18n("26th") << i18n("27th") << i18n("28th") << i18n("29th") << i18n("30th") << i18n("31st");
    }
}
