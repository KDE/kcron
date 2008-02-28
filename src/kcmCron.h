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

#include <QPixmap>
#include <QString>
#include <QWidget>
#include <QVariantList>
#include <QStringList>

#include <kcmodule.h>

class KCMCron : public KCModule {
	Q_OBJECT

public:
	//, const QVariantList& arguments
	KCMCron(QWidget* parent, const QStringList& args);

	~KCMCron();

	virtual void load();
	virtual void save();
	virtual void defaults();

};

#endif // KCM_CRON_H
