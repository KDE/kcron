/*
    KT icons implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcmCron.h"

#include <KAboutData>
#include <KConfig>
#include <KLocalizedString>
#include <KMessageBox>
#include <KPluginFactory>
#include <KStandardShortcut>
#include <QVBoxLayout>

#include "crontabWidget.h"

#include "ctInitializationError.h"
#include "ctcron.h"
#include "cthost.h"
#include "cttask.h"
#include "kcm_cron_debug.h"

K_PLUGIN_FACTORY(KCMCronFactory, registerPlugin<KCMCron>();)

KCMCron::KCMCron(QWidget *parent, const QVariantList & /*args*/)
    : KCModule(parent)
{
    auto aboutData = new KAboutData(QStringLiteral("kcm_cron"),
                                    i18n("Task Scheduler"),
                                    QStringLiteral("5.0"),
                                    i18n("KDE Task Scheduler"),
                                    KAboutLicense::GPL,
                                    i18n("(c) 2008, Nicolas Ternisien\n(c) 1999-2000, Gary Meyer"));

    aboutData->addAuthor(i18n("Nicolas Ternisien"), QString(), QStringLiteral("nicolas.ternisien@gmail.com"));
    aboutData->addAuthor(i18n("Gary Meyer"), QString(), QStringLiteral("gary@meyer.net"));
    aboutData->addAuthor(i18n("Robert Berry"), QString(), QStringLiteral("rjmber@ntlworld.com"));
    aboutData->addAuthor(i18n("James Ots"), QString(), QStringLiteral("code@jamesots.com"));
    aboutData->addAuthor(i18n("Alberto G. Hierro"), QString(), QStringLiteral("alberto.hierro@kdemail.net"));

    setAboutData(aboutData);

    // Initialize document.
    CTInitializationError ctInitializationError;
    mCtHost = new CTHost(findCrontabBinary(), ctInitializationError);
    if (ctInitializationError.hasErrorMessage()) {
        KMessageBox::error(this,
                           i18n("The following error occurred while initializing KCron:"
                                "\n\n%1\n\nKCron will now exit.\n",
                                ctInitializationError.errorMessage()));
    }

    mCrontabWidget = new CrontabWidget(this, mCtHost);

    qCDebug(KCM_CRON_LOG) << "Crontab Widget initialized";

    connect(mCrontabWidget->tasksWidget(), SIGNAL(taskModified(bool)), this, SIGNAL(changed(bool)));
    connect(mCrontabWidget->variablesWidget(), SIGNAL(variableModified(bool)), this, SIGNAL(changed(bool)));

    // Initialize view.
    auto layout = new QVBoxLayout(this);

    layout->addWidget(mCrontabWidget);

    init();
}

QString KCMCron::findCrontabBinary()
{
    return QStringLiteral(CRONTAB_BINARY);
}

KCMCron::~KCMCron()
{
    delete mCrontabWidget;
    delete mCtHost;
}

void KCMCron::load()
{
    qCDebug(KCM_CRON_LOG) << "Calling load";

    mCtHost->cancel();
}

void KCMCron::save()
{
    qCDebug(KCM_CRON_LOG) << "Saving crontab...";

    CTSaveStatus saveStatus = mCtHost->save(mCrontabWidget);
    if (saveStatus.isError()) {
        KMessageBox::detailedError(this, saveStatus.errorMessage(), saveStatus.detailErrorMessage());
    }
}

void KCMCron::defaults()
{
    qCDebug(KCM_CRON_LOG) << "Loading defaults";

    mCtHost->cancel();
}

bool KCMCron::init()
{
    // Display greeting screen.
    // If there currently are no scheduled tasks...
    int taskCount = 0;
    for (CTCron *ctCron : std::as_const(mCtHost->mCrons)) {
        taskCount += ctCron->tasks().count();
    }

    if (taskCount == 0) {
        show();
        // TODO Add this as a passive popup/message/something else
        KMessageBox::information(this,
                                 i18n("You can use this application to schedule programs to run in the background.\nTo schedule a new task now, click on "
                                      "the Tasks folder and select Edit/New from the menu."),
                                 i18n("Welcome to the Task Scheduler"),
                                 QStringLiteral("welcome"));
    }

    return true;
}

CTHost *KCMCron::ctHost() const
{
    return mCtHost;
}

#include "kcmCron.moc"
