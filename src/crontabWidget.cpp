/*
    KT main GUI view implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "crontabWidget.h"

#include <stdlib.h>
#include <unistd.h>

#include <QApplication>
#include <QButtonGroup>
#include <QClipboard>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QSplitter>
#include <QVBoxLayout>

#include <KLocalizedString>
#include <KStandardAction>
#include <QAction>

#include "ctcron.h"
#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "crontabPrinter.h"
#include "taskWidget.h"

#include "variableWidget.h"

#include "kcm_cron_debug.h"

CrontabWidget::CrontabWidget(QWidget *parent, CTHost *ctHost)
    : QWidget(parent)
{
    mCtHost = ctHost;

    setupActions();

    initialize();

    qCDebug(KCM_CRON_LOG) << "Clipboard Status " << hasClipboardContent();

    mTasksWidget->setFocus();

    QTreeWidgetItem *item = mTasksWidget->treeWidget()->topLevelItem(0);
    if (item != nullptr) {
        qCDebug(KCM_CRON_LOG) << "First item found" << mTasksWidget->treeWidget()->topLevelItemCount();
        item->setSelected(true);
    }

    mTasksWidget->changeCurrentSelection();
    mVariablesWidget->changeCurrentSelection();
}

CrontabWidget::~CrontabWidget()
{
    delete mTasksWidget;
    delete mVariablesWidget;
}

bool CrontabWidget::hasClipboardContent()
{
    if (!mClipboardTasks.isEmpty()) {
        return true;
    }

    if (!mClipboardVariables.isEmpty()) {
        return true;
    }

    return false;
}

QHBoxLayout *CrontabWidget::createCronSelector()
{
    auto layout = new QHBoxLayout();

    layout->addWidget(new QLabel(i18n("Show the following Cron:"), this));

    auto group = new QButtonGroup(this);

    mCurrentUserCronRadio = new QRadioButton(i18n("Personal Cron"), this);
    mCurrentUserCronRadio->setChecked(true);
    group->addButton(mCurrentUserCronRadio);
    layout->addWidget(mCurrentUserCronRadio);

    mSystemCronRadio = new QRadioButton(i18n("System Cron"), this);
    group->addButton(mSystemCronRadio);
    layout->addWidget(mSystemCronRadio);

    connect(group, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &CrontabWidget::refreshCron);

    layout->addStretch(1);

    return layout;
}

void CrontabWidget::initialize()
{
    auto layout = new QVBoxLayout(this);

    qCDebug(KCM_CRON_LOG) << "Begin view refresh";

    qCDebug(KCM_CRON_LOG) << "Creating Tasks list...";

    QHBoxLayout *cronSelector = createCronSelector();
    layout->addLayout(cronSelector);

    auto splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Vertical);
    layout->addWidget(splitter);

    mTasksWidget = new TasksWidget(this);
    splitter->addWidget(mTasksWidget);
    splitter->setStretchFactor(0, 2);

    mVariablesWidget = new VariablesWidget(this);
    splitter->addWidget(mVariablesWidget);
    splitter->setStretchFactor(1, 1);

    refreshCron();
}

void CrontabWidget::refreshCron()
{
    // Refreshes the main GUI.
    CTCron *ctCron = currentCron();

    mTasksWidget->refreshTasks(ctCron);
    mVariablesWidget->refreshVariables(ctCron);


    mTasksWidget->treeWidget()->setEnabled(true);
    mVariablesWidget->treeWidget()->setEnabled(true);

    toggleNewEntryActions(true);
    togglePasteAction(hasClipboardContent());
}

void CrontabWidget::copy()
{
    qDeleteAll(mClipboardTasks);
    mClipboardTasks.clear();

    qDeleteAll(mClipboardVariables);
    mClipboardVariables.clear();

    QString clipboardText;

    if (mTasksWidget->treeWidget()->hasFocus()) {
        qCDebug(KCM_CRON_LOG) << "Tasks copying";

        const QList<TaskWidget *> tasksWidget = mTasksWidget->selectedTasksWidget();
        for (TaskWidget *taskWidget : tasksWidget) {
            auto task = new CTTask(*(taskWidget->getCTTask()));
            mClipboardTasks.append(task);

            clipboardText += task->exportTask() + QLatin1String("\n");
        }
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        qCDebug(KCM_CRON_LOG) << "Variables copying";

        const QList<VariableWidget *> variablesWidget = mVariablesWidget->selectedVariablesWidget();
        for (VariableWidget *variableWidget : variablesWidget) {
            auto variable = new CTVariable(*(variableWidget->getCTVariable()));
            mClipboardVariables.append(variable);

            clipboardText += variable->exportVariable() + QLatin1String("\n");
        }
    }

    QApplication::clipboard()->setText(clipboardText, QClipboard::Clipboard);
    QApplication::clipboard()->setText(clipboardText, QClipboard::Selection);

    qCDebug(KCM_CRON_LOG) << "Clipboard Status " << hasClipboardContent();
    togglePasteAction(hasClipboardContent());
}

void CrontabWidget::cut()
{
    qCDebug(KCM_CRON_LOG) << "Cut content";

    copy();

    if (mTasksWidget->treeWidget()->hasFocus()) {
        qCDebug(KCM_CRON_LOG) << "Tasks cutting";
        mTasksWidget->deleteSelection();
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        qCDebug(KCM_CRON_LOG) << "Variables cutting";
        mVariablesWidget->deleteSelection();
    }
}

void CrontabWidget::paste()
{
    qCDebug(KCM_CRON_LOG) << "Paste content";

    if (mTasksWidget->treeWidget()->hasFocus()) {
        for (CTTask *task : std::as_const(mClipboardTasks)) {
            mTasksWidget->addTask(new CTTask(*task));
        }
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        for (CTVariable *variable : std::as_const(mClipboardVariables)) {
            mVariablesWidget->addVariable(new CTVariable(*variable));
        }
    }
}

CTCron *CrontabWidget::currentCron() const
{
    // Checks which mode the gui is in, either user cron or system cron,
    // returning the appropriate cron.
    if (mCurrentUserCronRadio->isChecked()) {
        return mCtHost->findCurrentUserCron();
    } else {
        return mCtHost->findSystemCron();
    }
}

TasksWidget *CrontabWidget::tasksWidget() const
{
    return mTasksWidget;
}

VariablesWidget *CrontabWidget::variablesWidget() const
{
    return mVariablesWidget;
}

CTHost *CrontabWidget::ctHost() const
{
    return mCtHost;
}

void CrontabWidget::checkOtherUsers()
{
    mOtherUserCronRadio->setChecked(true);

    refreshCron();
}

void CrontabWidget::setupActions()
{
    qCDebug(KCM_CRON_LOG) << "Setup actions";

    // Edit menu
    mCutAction = KStandardAction::cut(this, SLOT(cut()), this);
    mCopyAction = KStandardAction::copy(this, SLOT(copy()), this);
    mPasteAction = KStandardAction::paste(this, SLOT(paste()), this);
    togglePasteAction(false);

    qCDebug(KCM_CRON_LOG) << "Actions initialized";
}

QList<QAction *> CrontabWidget::cutCopyPasteActions()
{
    QList<QAction *> actions;
    actions.append(mCutAction);
    actions.append(mCopyAction);
    actions.append(mPasteAction);

    return actions;
}

void CrontabWidget::togglePasteAction(bool state)
{
    mPasteAction->setEnabled(state);
}

void CrontabWidget::toggleModificationActions(bool state)
{
    mCutAction->setEnabled(state);
    mCopyAction->setEnabled(state);

    mTasksWidget->toggleModificationActions(state);
    mVariablesWidget->toggleModificationActions(state);
}

void CrontabWidget::toggleNewEntryActions(bool state)
{
    mTasksWidget->toggleNewEntryAction(state);
    mVariablesWidget->toggleNewEntryAction(state);
}

void CrontabWidget::print()
{
    CrontabPrinter printer(this);

    if (!printer.start()) {
        qCDebug(KCM_CRON_LOG) << "Unable to start printer";
        return;
    }
    printer.printTasks();
    printer.printVariables();

    printer.finish();
}

#include "moc_crontabWidget.cpp"
