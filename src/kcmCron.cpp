/***************************************************************************
 *   KT icons implementation.                                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "kcmCron.h"

#include <QImage>
#include <QPixmap>

#include <kglobal.h>
#include <kgenericfactory.h>
#include <kaboutdata.h>

#include <QComboBox>

#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"

#include "crontabWidget.h"

#include "logging.h"

typedef KGenericFactory<KCMCron, QWidget> KCMCronFactory;
K_EXPORT_COMPONENT_FACTORY( kcm_cron, KCMCronFactory("kcron.po") );



KCMCron::KCMCron(QWidget* parent, const QStringList& /*args*/) : 
	KCModule(KCMCronFactory::componentData(), parent) {

	logDebug() << "KCMCRON CONSTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON CONSTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON CONSTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON CONSTRUCTOR !!!!!!!!!!!!!" << endl;
	
	KAboutData* aboutData = new KAboutData("kcm_cron", 0, ki18n("Task Scheduler"),
			KDE_VERSION_STRING, ki18n("KDE Task Scheduler"), KAboutData::License_GPL, ki18n("(c) 2008, Nicolas Ternisien\n(c) 1999-2000, Gary Meyer"));

	aboutData->addAuthor(ki18n("Nicolas Ternisien"), KLocalizedString(), "nicolas.ternisien@gmail.com");
	aboutData->addAuthor(ki18n("Gary Meyer"), KLocalizedString(), "gary@meyer.net");
	aboutData->addAuthor(ki18n("Robert Berry"), KLocalizedString(), "rjmber@ntlworld.com");
	aboutData->addAuthor(ki18n("James Ots"), KLocalizedString(), "code@jamesots.com");
	aboutData->addAuthor(ki18n("Alberto G. Hierro"), KLocalizedString(), "alberto.hierro@kdemail.net");

	setAboutData(aboutData);

}

KCMCron::~KCMCron() {
	logDebug() << "KCMCRON DESTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON DESTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON DESTRUCTOR !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON DESTRUCTOR !!!!!!!!!!!!!" << endl;
	
}

void KCMCron::load() {
	logDebug() << "KCMCRON load !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON load !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON load !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON load !!!!!!!!!!!!!" << endl;
}

void KCMCron::save() {
	logDebug() << "KCMCRON save !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON save !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON save !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON save !!!!!!!!!!!!!" << endl;
	
	
}

void KCMCron::defaults() {
	logDebug() << "KCMCRON defaults !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON defaults !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON defaults !!!!!!!!!!!!!" << endl;
	logDebug() << "KCMCRON defaults !!!!!!!!!!!!!" << endl;
	
}

#include "kcmCron.moc"
