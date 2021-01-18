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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSplitter>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QApplication>
#include <QClipboard>

#include <KLocalizedString>
#include <QIcon>
#include <QAction>
#include <KStandardAction>

#include "cthost.h"
#include "ctcron.h"
#include "ctvariable.h"
#include "cttask.h"
#include "ctGlobalCron.h"

#include "crontabPrinter.h"
#include "taskWidget.h"

#include "variableWidget.h"

#include "kcmCron.h"

#include "logging.h"

class CTGlobalCron;

CrontabWidget::CrontabWidget(QWidget *parent, CTHost *ctHost)
    : QWidget(parent)
{
    mCtHost = ctHost;

    if (mCtHost->isRootUser()) {
        mCtGlobalCron = new CTGlobalCron(mCtHost);
    } else {
        mCtGlobalCron = nullptr;
    }

    setupActions();

    initialize();

    logDebug() << "Clipboard Status " << hasClipboardContent();

    mTasksWidget->setFocus();

    QTreeWidgetItem *item = mTasksWidget->treeWidget()->topLevelItem(0);
    if (item != nullptr) {
        logDebug() << "First item found" << mTasksWidget->treeWidget()->topLevelItemCount();
        item->setSelected(true);
    }

    mTasksWidget->changeCurrentSelection();
    mVariablesWidget->changeCurrentSelection();
}

CrontabWidget::~CrontabWidget()
{
    delete mTasksWidget;
    delete mVariablesWidget;

    delete mCtGlobalCron;
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
    QHBoxLayout *layout = new QHBoxLayout();

    layout->addWidget(new QLabel(i18n("Show the following Cron:"), this));

    QButtonGroup *group = new QButtonGroup(this);

    mCurrentUserCronRadio = new QRadioButton(i18n("Personal Cron"), this);
    mCurrentUserCronRadio->setChecked(true);
    group->addButton(mCurrentUserCronRadio);
    layout->addWidget(mCurrentUserCronRadio);

    mSystemCronRadio = new QRadioButton(i18n("System Cron"), this);
    group->addButton(mSystemCronRadio);
    layout->addWidget(mSystemCronRadio);

    mOtherUserCronRadio = new QRadioButton(i18n("Cron of User:"), this);
    group->addButton(mOtherUserCronRadio);

    mOtherUsers = new QComboBox(this);

    layout->addWidget(mOtherUserCronRadio);
    layout->addWidget(mOtherUsers);

    if (ctHost()->isRootUser()) {
        QStringList users;

        const auto crons = ctHost()->mCrons;
        for (CTCron *ctCron : crons) {
            if (ctCron->isCurrentUserCron()) {
                continue;
            }

            if (ctCron->isSystemCron()) {
                continue;
            }

            users.append(ctCron->userLogin());
        }

        users.sort();
        mOtherUsers->addItems(users);
        mOtherUsers->addItem(QIcon::fromTheme(QStringLiteral("users")), i18n("Show All Personal Crons"));
    } else {
        mOtherUserCronRadio->hide();
        mOtherUsers->hide();
    }

    connect(group, static_cast<void (QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked), this, &CrontabWidget::refreshCron);
    connect(mOtherUsers, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &CrontabWidget::checkOtherUsers);

    layout->addStretch(1);

    return layout;
}

void CrontabWidget::initialize()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    logDebug() << "Begin view refresh";

    logDebug() << "Creating Tasks list...";

    QHBoxLayout *cronSelector = createCronSelector();
    layout->addLayout(cronSelector);

    QSplitter *splitter = new QSplitter(this);
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
    CTCron *ctCron = currentCron();

    mTasksWidget->refreshTasks(ctCron);
    mVariablesWidget->refreshVariables(ctCron);

    if (ctCron->isMultiUserCron() && !ctHost()->isRootUser()) {
        logDebug() << "Disabling view...";

        mTasksWidget->treeWidget()->setEnabled(false);
        mVariablesWidget->treeWidget()->setEnabled(false);

        toggleNewEntryActions(false);
        toggleModificationActions(false);
        togglePasteAction(false);
        mTasksWidget->toggleRunNowAction(false);
    } else {
        logDebug() << "Enabling view...";

        mTasksWidget->treeWidget()->setEnabled(true);
        mVariablesWidget->treeWidget()->setEnabled(true);

        toggleNewEntryActions(true);
        togglePasteAction(hasClipboardContent());
    }
}

void CrontabWidget::copy()
{
    foreach (CTTask *task, mClipboardTasks) {
        delete task;
    }
    mClipboardTasks.clear();

    foreach (CTVariable *variable, mClipboardVariables) {
        delete variable;
    }
    mClipboardVariables.clear();

    QString clipboardText;

    if (mTasksWidget->treeWidget()->hasFocus()) {
        logDebug() << "Tasks copying";

        const QList<TaskWidget *> tasksWidget = mTasksWidget->selectedTasksWidget();
        for (TaskWidget *taskWidget : tasksWidget) {
            CTTask *task = new CTTask(*(taskWidget->getCTTask()));
            mClipboardTasks.append(task);

            clipboardText += task->exportTask() + QLatin1String("\n");
        }
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        logDebug() << "Variables copying";

        const QList<VariableWidget *> variablesWidget = mVariablesWidget->selectedVariablesWidget();
        for (VariableWidget *variableWidget : variablesWidget) {
            CTVariable *variable = new CTVariable(*(variableWidget->getCTVariable()));
            mClipboardVariables.append(variable);

            clipboardText += variable->exportVariable() + QLatin1String("\n");
        }
    }

    QApplication::clipboard()->setText(clipboardText, QClipboard::Clipboard);
    QApplication::clipboard()->setText(clipboardText, QClipboard::Selection);

    logDebug() << "Clipboard Status " << hasClipboardContent();
    togglePasteAction(hasClipboardContent());
}

void CrontabWidget::cut()
{
    logDebug() << "Cut content";

    copy();

    if (mTasksWidget->treeWidget()->hasFocus()) {
        logDebug() << "Tasks cutting";
        mTasksWidget->deleteSelection();
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        logDebug() << "Variables cutting";
        mVariablesWidget->deleteSelection();
    }
}

void CrontabWidget::paste()
{
    logDebug() << "Paste content";

    if (mTasksWidget->treeWidget()->hasFocus()) {
        for (CTTask *task : qAsConst(mClipboardTasks)) {
            mTasksWidget->addTask(new CTTask(*task));
        }
    }

    if (mVariablesWidget->treeWidget()->hasFocus()) {
        for (CTVariable *variable : qAsConst(mClipboardVariables)) {
            mVariablesWidget->addVariable(new CTVariable(*variable));
        }
    }
}

CTCron *CrontabWidget::currentCron() const
{
    if (mCurrentUserCronRadio->isChecked()) {
        return mCtHost->findCurrentUserCron();
    } else if (mSystemCronRadio->isChecked()) {
        return mCtHost->findSystemCron();
    }

    if (mOtherUsers->currentIndex() == mOtherUsers->count()-1) {
        logDebug() << "Using Global Cron";
        return mCtGlobalCron;
    }

    QString currentUserLogin = mOtherUsers->currentText();
    return mCtHost->findUserCron(currentUserLogin);
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
    logDebug() << "Setup actions";

    //Edit menu
    mCutAction = KStandardAction::cut(this, SLOT(cut()), this);
    mCopyAction = KStandardAction::copy(this, SLOT(copy()), this);
    mPasteAction = KStandardAction::paste(this, SLOT(paste()), this);
    togglePasteAction(false);

    logDebug() << "Actions initialized";
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
        logDebug() << "Unable to start printer";
        return;
    }
    printer.printTasks();
    printer.printVariables();

    printer.finish();
}
