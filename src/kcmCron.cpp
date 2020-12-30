/*
    KT icons implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcmCron.h"




#include <KPluginFactory>
#include <KAboutData>
#include <QVBoxLayout>
#include <KStandardShortcut>
#include <KMessageBox>
#include <KConfig>
#include <KLocalizedString>
#include <KToggleAction>
#include <QIcon>
#include <KConfigGroup>

#include "crontabWidget.h"

#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"
#include "ctInitializationError.h"
#include "logging.h"

K_PLUGIN_FACTORY(KCMCronFactory, registerPlugin<KCMCron>();)

class KCMCronPrivate {
public:

	/**
	 * Main GUI view/working area.
	 */
	CrontabWidget* crontabWidget;

	/**
	 * Document object, here crotab entries.
	 */
	CTHost* ctHost;

};



KCMCron::KCMCron(QWidget* parent, const QVariantList& /*args*/) :
	KCModule(parent),
	d(new KCMCronPrivate()) {


        KAboutData* aboutData = new KAboutData(QStringLiteral("kcm_cron"), i18n("Task Scheduler"),
                        QStringLiteral("5.0"), i18n("KDE Task Scheduler"), KAboutLicense::GPL, i18n("(c) 2008, Nicolas Ternisien\n(c) 1999-2000, Gary Meyer"));

        aboutData->addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
        aboutData->addAuthor(i18n("Gary Meyer"), QString(), QStringLiteral("gary@meyer.net"));
        aboutData->addAuthor(i18n("Robert Berry"), QString(), QStringLiteral("rjmber@ntlworld.com"));
        aboutData->addAuthor(i18n("James Ots"), QString(), QStringLiteral("code@jamesots.com"));
        aboutData->addAuthor(i18n("Alberto G. Hierro"), QString(), QStringLiteral("alberto.hierro@kdemail.net"));

	setAboutData(aboutData);


	// Initialize document.
	CTInitializationError ctInitializationError;
	d->ctHost = new CTHost(findCrontabBinary(), ctInitializationError);
	if (ctInitializationError.hasErrorMessage()) {
		KMessageBox::error(this, i18n("The following error occurred while initializing KCron:"
			"\n\n%1\n\nKCron will now exit.\n", ctInitializationError.errorMessage()));
	}

	d->crontabWidget = new CrontabWidget(this, d->ctHost);

    logDebug() << "Crontab Widget initialized";

	connect(d->crontabWidget->tasksWidget(), SIGNAL(taskModified(bool)), this, SIGNAL(changed(bool)));
	connect(d->crontabWidget->variablesWidget(), SIGNAL(variableModified(bool)), this, SIGNAL(changed(bool)));

	// Initialize view.
	QVBoxLayout* layout = new QVBoxLayout(this);

	layout->addWidget(d->crontabWidget);

	init();

}

QString KCMCron::findCrontabBinary() {
	return QStringLiteral( CRONTAB_BINARY );
}

KCMCron::~KCMCron() {
	delete d->crontabWidget;
	delete d->ctHost;

	delete d;
}

void KCMCron::load() {
    logDebug() << "Calling load";

	d->ctHost->cancel();
}

void KCMCron::save() {
    logDebug() << "Saving crontab...";

	CTSaveStatus saveStatus = d->ctHost->save();
	if (saveStatus.isError() == true) {
		KMessageBox::detailedError(this, saveStatus.errorMessage(), saveStatus.detailErrorMessage());
	}

}

void KCMCron::defaults() {
    logDebug() << "Loading defaults";

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
			KMessageBox::information(this, i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on the Tasks folder and select Edit/New from the menu."), i18n("Welcome to the Task Scheduler"), QStringLiteral( "welcome" ));
		}
	}

	return true;
}

CTHost* KCMCron::ctHost() const {
	return d->ctHost;
}

#include "kcmCron.moc"
