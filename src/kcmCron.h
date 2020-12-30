/*
    KT icons.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KCM_CRON_H
#define KCM_CRON_H

/**
 * Crontab binary executable location
 * The $PATH variable could be used
 */
#define CRONTAB_BINARY "crontab"

#include <QString>
#include <QWidget>
#include <QVariantList>

#include <KCModule>
#include <KSharedConfig>

class CTHost;

class KCMCronPrivate;

class KCMCron : public KCModule {
	Q_OBJECT

public:
	//, const QVariantList& arguments
	KCMCron(QWidget* parent, const QVariantList& args);

	~KCMCron();

	void load() Q_DECL_OVERRIDE;
	void save() Q_DECL_OVERRIDE;
	void defaults() Q_DECL_OVERRIDE;
	
	/**
	 * Additional init
	 */
	bool init();

	/**
	 * Returns a reference to the CTHost.
	 */
	CTHost* ctHost() const;
	
	QString findCrontabBinary();
	
private:

	KCMCronPrivate* const d;
};

#endif // KCM_CRON_H
