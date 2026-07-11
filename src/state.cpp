/*
 *  SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "state.h"

#include <KConfig>
#include <KConfigGroup>

#include <QSettings>

State::State()
    : QObject()
{
    QSettings settings;
    if (settings.contains("viewState")) {
        setViewState(settings.value("viewState").toByteArray());
        settings.remove("viewState");
    }
}

QByteArray State::viewState()
{
    const KConfig stateConfig(QStringLiteral("kcronstaterc"), KConfig::SimpleConfig, QStandardPaths::GenericStateLocation);
    return stateConfig.group(QStringLiteral("General")).readEntry("viewState", QByteArray());
}

void State::setViewState(const QByteArray &state)
{
    KConfig stateConfig(QStringLiteral("kcronstaterc"), KConfig::SimpleConfig, QStandardPaths::GenericStateLocation);
    stateConfig.group(QStringLiteral("General")).writeEntry("viewState", state);

    Q_EMIT viewStateChanged();
}
