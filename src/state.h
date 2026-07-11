/*
 *  SPDX-FileCopyrightText: 2026 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <QObject>

class State : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray viewState READ viewState WRITE setViewState NOTIFY viewStateChanged)

public:
    State();

    QByteArray viewState();
    void setViewState(const QByteArray &state);
    Q_SIGNAL void viewStateChanged();
};
