/*
    KT icons.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

/**
 * Crontab binary executable location
 * The $PATH variable could be used
 */
#define CRONTAB_BINARY "crontab"

#include <QString>
#include <QVariantList>
#include <QWidget>

#include <KCModule>
#include <KSharedConfig>

class CTHost;
class CrontabWidget;

class KCMCron : public KCModule
{
    Q_OBJECT

public:
    KCMCron(QObject *parent);

    ~KCMCron() override;

    void load() override;
    void save() override;
    void defaults() override;

    /**
     * Additional init
     */
    bool init();

    /**
     * Returns a reference to the CTHost.
     */
    CTHost *ctHost() const;

    QString findCrontabBinary();

private:
    /**
     * Main GUI view/working area.
     */
    CrontabWidget *mCrontabWidget = nullptr;

    /**
     * Document object, here crotab entries.
     */
    CTHost *mCtHost = nullptr;
};

