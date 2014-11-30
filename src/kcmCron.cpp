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




#include <kpluginfactory.h>
#include <kaboutdata.h>
#include <QVBoxLayout>
#include <kstandardshortcut.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <KLocalizedString>
#include <kiconloader.h>
#include <ktoggleaction.h>
#include <kxmlguifactory.h>
#include <ktoolbar.h>
#include <QIcon>
#include <kconfiggroup.h>

#include "kcronIcons.h"
#include "crontabWidget.h"

#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"
#include "ctInitializationError.h"
#include "logging.h"

K_PLUGIN_FACTORY(KCMCronFactory, registerPlugin<KCMCron>();)
K_EXPORT_PLUGIN(KCMCronFactory("kcron"))

class KCMCronPrivate {
public:

	/**
	 * Main GUI view/working area.
	 */
	CrontabWidget* crontabWidget;

	/**
	 * Document object, here crotab enries.
	 */
	CTHost* ctHost;

};



KCMCron::KCMCron(QWidget* parent, const QVariantList& /*args*/) :
	KCModule(parent),
	d(new KCMCronPrivate()) {


	KAboutData* aboutData = new KAboutData("kcm_cron", i18n("Task Scheduler"),
			"5.0", i18n("KDE Task Scheduler"), KAboutLicense::GPL, i18n("(c) 2008, Nicolas Ternisien\n(c) 1999-2000, Gary Meyer"));

	aboutData->addAuthor(i18n("Nicolas Ternisien"), QString(), "nicolas.ternisien@gmail.com");
	aboutData->addAuthor(i18n("Gary Meyer"), QString(), "gary@meyer.net");
	aboutData->addAuthor(i18n("Robert Berry"), QString(), "rjmber@ntlworld.com");
	aboutData->addAuthor(i18n("James Ots"), QString(), "code@jamesots.com");
	aboutData->addAuthor(i18n("Alberto G. Hierro"), QString(), "alberto.hierro@kdemail.net");

	setAboutData(aboutData);


	// Initialize document.
	CTInitializationError ctInitializationError;
	d->ctHost = new CTHost(findCrontabBinary(), ctInitializationError);
	if (ctInitializationError.hasErrorMessage()) {
		KMessageBox::error(this, i18n("The following error occurred while initializing KCron:"
			"\n\n%1\n\nKCron will now exit.\n", ctInitializationError.errorMessage()));
	}

	d->crontabWidget = new CrontabWidget(this, d->ctHost);

	logDebug() << "Crontab Widget initialized" << endl;

	connect(d->crontabWidget->tasksWidget(), SIGNAL(taskModified(bool)), this, SIGNAL(changed(bool)));
	connect(d->crontabWidget->variablesWidget(), SIGNAL(variableModified(bool)), this, SIGNAL(changed(bool)));

	// Initialize view.
	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(d->crontabWidget);

	init();

}

QString KCMCron::findCrontabBinary() {
	return QLatin1String( CRONTAB_BINARY );
}

KCMCron::~KCMCron() {
	delete d->crontabWidget;
	delete d->ctHost;

	delete d;
}

void KCMCron::load() {
	logDebug() << "Calling load" << endl;

	d->ctHost->cancel();
}

void KCMCron::save() {
	logDebug() << "Saving crontab..." << endl;

	CTSaveStatus saveStatus = d->ctHost->save();
	if (saveStatus.isError() == true) {
		KMessageBox::detailedError(this, saveStatus.errorMessage(), saveStatus.detailErrorMessage());
	}

}

void KCMCron::defaults() {
	logDebug() << "Loading defaults" << endl;

	d->ctHost->cancel();
}


bool KCMCron::init() {

	// Display greeting screen.
	// if there currently are no scheduled tasks...
	if (!d->ctHost->isRootUser()) {
		int taskCount = 0;
		foreach(CTCron* ctCron, d->ctHost->crons) {
			taskCount += ctCron->tasks().count();
		}

		if (taskCount == 0) {
			show();
			//TODO Add this as a passive popup/message/something else
			KMessageBox::information(this, i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on the Tasks folder and select Edit/New from the menu."), i18n("Welcome to the Task Scheduler"), QLatin1String( "welcome" ));
		}
	}

	return true;
}

CTHost* KCMCron::ctHost() const {
	return d->ctHost;
}

#include "kcmCron.moc"
