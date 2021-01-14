/*
    CT Unit of Time Interval Implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "ctunit.h"

#include <KLocalizedString>

#include "logging.h"

CTUnit::CTUnit(int _min, int _max, const QString &tokStr)
{
    mMin = _min;
    mMax = _max;
    initialize(tokStr);
}

CTUnit::CTUnit(const CTUnit &source)
{
    mMin = source.mMin;
    mMax = source.mMax;

    mInitialEnabled.clear();
    mEnabled.clear();
    for (int i = 0; i <= mMax; i++) {
        mInitialEnabled.append(false);
        mEnabled.append(source.mEnabled.at(i));
    }

    mInitialTokStr = QLatin1String("");
    mDirty = true;
}

CTUnit::~CTUnit()
{
}

CTUnit &CTUnit::operator =(const CTUnit &unit)
{
    if (this == &unit) {
        return *this;
    }

    mMin = unit.mMin;
    mMax = unit.mMax;

    mEnabled.clear();
    for (int i = 0; i <= mMax; i++) {
        mEnabled.append(unit.mEnabled[i]);
    }
    mDirty = true;

    return *this;
}

void CTUnit::initialize(const QString &tokStr)
{
    mEnabled.clear();
    for (int i = 0; i <= mMax; i++) {
        mEnabled.append(false);
        mInitialEnabled.append(false);
    }

    for (int i = mMin; i <= mMax; i++) {
        mInitialEnabled[i] = mEnabled[i];
    }

    parse(tokStr);
    mInitialTokStr = tokStr;
    mDirty = false;
}

void CTUnit::parse(const QString &tokenString)
{
    QString tokStr = tokenString;

    // subelement is that which is between commas
    QString subelement;
    int commapos, slashpos, dashpos;
    int beginat, endat, step;

    // loop through each subelement
    tokStr += QLatin1Char(',');
    while ((commapos = tokStr.indexOf(QLatin1String(","))) > 0) {
        subelement = tokStr.mid(0, commapos);

        // find "/" to determine step
        slashpos = subelement.indexOf(QLatin1String("/"));
        if (slashpos == -1) {
            step = 1;
            slashpos = subelement.length();
        } else {
            step = fieldToValue(subelement.mid(slashpos+1, subelement.length()-slashpos-1));
            if (step < 1) {
                step = 1;
            }
        }

        // find "=" to determine range
        dashpos = subelement.indexOf(QLatin1String("-"));
        if (dashpos == -1) {
            // deal with "*"
            if (subelement.mid(0, slashpos) == QLatin1String("*")) {
                beginat = mMin;
                endat = mMax;
            } else {
                beginat = fieldToValue(subelement.mid(0, slashpos));
                endat = beginat;
            }
        } else {
            beginat = fieldToValue(subelement.mid(0, dashpos));
            endat = fieldToValue(subelement.mid(dashpos+1, slashpos-dashpos-1));
        }

        // ignore out of range
        if (beginat < 0) {
            beginat = 0;
        }
        if (endat > mMax) {
            endat = mMax;
        }

        // setup enabled
        for (int i = beginat; i <= endat; i += step) {
            mInitialEnabled[i] = mEnabled[i] = true;
        }

        tokStr = tokStr.mid(commapos+1, tokStr.length()-commapos-1);
    }

    return;
}

QString CTUnit::exportUnit() const
{
    if (!mDirty) {
        return mInitialTokStr;
    }

    if (isAllEnabled()) {
        return QStringLiteral("*");
    }

    int total = enabledCount();
    int count = 0;
    QString tokenizeUnit;

    for (int num = mMin; num <= mMax; num++) {
        if (mEnabled[num]) {
            tokenizeUnit += QString::number(num);
            count++;

            if (count < total) {
                tokenizeUnit += QLatin1Char(',');
            }
        }
    }

    return tokenizeUnit;
}

QString CTUnit::genericDescribe(const QList<QString> &label) const
{
    int total(enabledCount());
    int count(0);
    QString tmpStr;
    for (int i = mMin; i <= mMax; i++) {
        if (mEnabled[i]) {
            tmpStr += label.at(i);
            count++;
            switch (total - count) {
            case 0:
                break;
            case 1:
                if (total > 2) {
                    tmpStr += i18n(",");
                }
                tmpStr += i18n(" and ");
                break;
            default:
                tmpStr += i18n(", ");
                break;
            }
        }
    }
    return tmpStr;
}

int CTUnit::minimum() const
{
    return mMin;
}

int CTUnit::maximum() const
{
    return mMax;
}

bool CTUnit::isEnabled(int pos) const
{
    return mEnabled.at(pos);
}

bool CTUnit::isAllEnabled() const
{
    for (int i = mMin; i <= mMax; i++) {
        if (mEnabled.at(i) == false) {
            return false;
        }
    }

    return true;
}

void CTUnit::setEnabled(int pos, bool value)
{
    mEnabled[pos] = value;
    mDirty = true;
    return;
}

bool CTUnit::isDirty() const
{
    return mDirty;
}

int CTUnit::enabledCount() const
{
    int total(0);
    for (int i = mMin; i <= mMax; i++) {
        total += (mEnabled[i] == true);
    }
    return total;
}

void CTUnit::apply()
{
    mInitialTokStr = exportUnit();
    for (int i = mMin; i <= mMax; i++) {
        mInitialEnabled[i] = mEnabled[i];
    }
    mDirty = false;
    return;
}

void CTUnit::cancel()
{
    for (int i = mMin; i <= mMax; i++) {
        mEnabled[i] = mInitialEnabled[i];
    }
    mDirty = false;
    return;
}

int CTUnit::fieldToValue(const QString &entry) const
{
    QString lower = entry.toLower();

    // check for days
    QList<QString> days;
    days << QStringLiteral("sun") << QStringLiteral("mon") << QStringLiteral("tue") << QStringLiteral("wed") << QStringLiteral("thu") << QStringLiteral("fri") << QStringLiteral("sat");

    int day = days.indexOf(lower);
    if (day != -1) {
        return day;
    }

    // check for months
    QList<QString> months;
    months << QLatin1String("") << QStringLiteral("jan") << QStringLiteral("feb") << QStringLiteral("mar") << QStringLiteral("apr") << QStringLiteral("may") << QStringLiteral("jun") << QStringLiteral("jul") << QStringLiteral("aug") << QStringLiteral("sep") << QStringLiteral("oct") << QStringLiteral(
        "nov") << QStringLiteral("dec");

    int month = months.indexOf(lower);
    if (month != -1) {
        return month;
    }

    //If the string does not match a day ora month, then it's a simple number (minute, hour or day of month)
    return entry.toInt();
}

/**
 * Find a period in enabled values
 * If no period has been found, return 0
 */
int CTUnit::findPeriod(const QList<int> &periods) const
{
    foreach (int period, periods) {
        bool validPeriod = true;

        for (int i = minimum(); i <= maximum(); i++) {
            bool periodTesting;
            if ((double)i/(double)period == i/period) {
                periodTesting = true;
            } else {
                periodTesting = false;
            }

            if (isEnabled(i) != periodTesting) {
                validPeriod = false;
                break;
            }
        }

        if (validPeriod) {
            return period;
        }
    }

    return 0;
}
