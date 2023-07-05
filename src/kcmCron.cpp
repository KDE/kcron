/*
    KT icons implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kcmCron.h"

#include <KAboutData>
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

K_PLUGIN_CLASS_WITH_JSON(KCMCron, "kcm_cron.json")

#if QT_VERSION_MAJOR == 5
KCMCron::KCMCron(QWidget *parent, const QVariantList & /*args*/)
#else
KCMCron::KCMCron(QObject *parent)
#endif
    : KCModule(parent)
{
    // Initialize document.
    CTInitializationError ctInitializationError;
    mCtHost = new CTHost(findCrontabBinary(), ctInitializationError);
    if (ctInitializationError.hasErrorMessage()) {
        KMessageBox::error(widget(),
                           i18n("The following error occurred while initializing KCron:"
                                "\n\n%1\n\nKCron will now exit.\n",
                                ctInitializationError.errorMessage()));
    }

    mCrontabWidget = new CrontabWidget(widget(), mCtHost);

    qCDebug(KCM_CRON_LOG) << "Crontab Widget initialized";

#if QT_VERSION_MAJOR == 5
    connect(mCrontabWidget->tasksWidget(), SIGNAL(taskModified(bool)), this, SIGNAL(changed(bool)));
    connect(mCrontabWidget->variablesWidget(), SIGNAL(variableModified(bool)), this, SIGNAL(changed(bool)));
#else
    connect(mCrontabWidget->tasksWidget(), &TasksWidget::taskModified, this, &KCModule::setNeedsSave);
    connect(mCrontabWidget->variablesWidget(), &VariablesWidget::variableModified, this, &KCModule::setNeedsSave);
#endif

    // Initialize view.
    auto layout = new QVBoxLayout(widget());

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
        KMessageBox::detailedError(widget(), saveStatus.errorMessage(), saveStatus.detailErrorMessage());
    }
    qCDebug(KCM_CRON_LOG) << "saved ct host";
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
        // TODO Add this as a passive popup/message/something else
        KMessageBox::information(widget(),
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

#include "moc_kcmCron.cpp"
