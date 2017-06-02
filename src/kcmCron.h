/***************************************************************************
 *   KT icons.                                                             *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

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

#include <kcmodule.h>
#include <ksharedconfig.h>

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
