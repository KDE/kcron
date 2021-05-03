/*
    KT list view item tasks implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "tasksWidget.h"

#include <QAction>
#include <QList>
#include <QProcess>

#include <KLocalizedString>
#include <KStandardAction>

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "crontabWidget.h"
#include "taskEditorDialog.h"
#include "taskWidget.h"

#include "logging.h"

/**
 * Construct tasks folder from branch.
 */
TasksWidget::TasksWidget(CrontabWidget *crontabWidget)
    : GenericListWidget(crontabWidget, i18n("<b>Scheduled Tasks</b>"), QIcon::fromTheme(QStringLiteral("system-run")))
{
    refreshHeaders();

    treeWidget()->sortItems(1, Qt::AscendingOrder);

    setupActions(crontabWidget);
    prepareContextualMenu();

    connect(treeWidget(), &QTreeWidget::itemSelectionChanged, this, &TasksWidget::changeCurrentSelection);

    logDebug() << "Tasks list created";
}

TasksWidget::~TasksWidget()
{
}

QList<TaskWidget *> TasksWidget::selectedTasksWidget() const
{
    QList<TaskWidget *> tasksWidget;

    const QList<QTreeWidgetItem *> tasksItems = treeWidget()->selectedItems();
    tasksWidget.reserve(tasksItems.count());
    for (QTreeWidgetItem *item : tasksItems) {
        auto taskWidget = static_cast<TaskWidget *>(item);
        tasksWidget.append(taskWidget);
    }

    return tasksWidget;
}

TaskWidget *TasksWidget::firstSelectedTaskWidget() const
{
    QTreeWidgetItem *item = firstSelected();
    if (!item) {
        return nullptr;
    }

    return static_cast<TaskWidget *>(item);
}

int TasksWidget::statusColumnIndex()
{
    if (needUserColumn()) {
        return 3;
    }

    return 2;
}

void TasksWidget::runTaskNow() const
{
    TaskWidget *taskWidget = firstSelectedTaskWidget();
    if (taskWidget == nullptr) {
        return;
    }

    CTCron *ctCron = crontabWidget()->currentCron();
    if (!ctCron) {
        logDebug() << "Unable to find the related CtCron, please report this bug to KCron developer";
        return;
    }

    const QString taskCommand = taskWidget->getCTTask()->command;

    const QString echoMessage = i18nc("Do not use any quote characters (') in this string", "End of script execution. Type Enter or Ctrl+C to exit.");
    QStringList commandList;

    const auto variables = ctCron->variables();
    commandList.reserve(variables.count() + 5);
    for (CTVariable *variable : variables) {
        commandList << QStringLiteral("export %1=\"%2\"").arg(variable->variable, variable->value);
    }

    commandList << taskCommand;
    commandList << QStringLiteral("echo '-------------------------------------------------------------------------'");
    commandList << QLatin1String("echo ") + echoMessage;
    commandList << QStringLiteral("echo '-------------------------------------------------------------------------'");
    commandList << QStringLiteral("read");

    QStringList parameters;
    parameters << QStringLiteral("-e") << QStringLiteral("bash") << QStringLiteral("-c");
    parameters << commandList.join(QLatin1Char(';'));

    QProcess process;
    process.startDetached(QStringLiteral("konsole"), parameters);
}

void TasksWidget::createTask()
{
    auto task =
        new CTTask(QLatin1String(""), QLatin1String(""), crontabWidget()->currentCron()->userLogin(), crontabWidget()->currentCron()->isMultiUserCron());

    TaskEditorDialog taskEditorDialog(task, i18n("New Task"), crontabWidget());
    const int result = taskEditorDialog.exec();

    if (result == QDialog::Accepted) {
        addTask(task);
        Q_EMIT taskModified(true);

        changeCurrentSelection();
    } else {
        delete task;
    }
}

void TasksWidget::addTask(CTTask *task)
{
    CTCron *cron = crontabWidget()->currentCron();

    cron->addTask(task);
    new TaskWidget(this, task);
}

void TasksWidget::modifySelection()
{
    modifySelection(firstSelectedTaskWidget(), -1);
}

void TasksWidget::modifySelection(QTreeWidgetItem *item, int position)
{
    auto taskWidget = static_cast<TaskWidget *>(item);
    if (taskWidget) {
        if (position == statusColumnIndex()) {
            taskWidget->toggleEnable();
            Q_EMIT taskModified(true);
        } else {
            CTTask *task = taskWidget->getCTTask();
            TaskEditorDialog taskEditorDialog(task, i18n("Modify Task"), crontabWidget());
            int result = taskEditorDialog.exec();

            if (result == QDialog::Accepted) {
                crontabWidget()->currentCron()->modifyTask(task);
                taskWidget->refresh();
                Q_EMIT taskModified(true);
            }
        }
    }

    logDebug() << "End of modification";
}

void TasksWidget::deleteSelection()
{
    logDebug() << "Selection deleting...";

    const QList<QTreeWidgetItem *> tasksItems = treeWidget()->selectedItems();

    bool deleteSomething = !(tasksItems.isEmpty());

    for (QTreeWidgetItem *item : tasksItems) {
        auto taskWidget = static_cast<TaskWidget *>(item);

        crontabWidget()->currentCron()->removeTask(taskWidget->getCTTask());
        delete taskWidget->getCTTask();
        treeWidget()->takeTopLevelItem(treeWidget()->indexOfTopLevelItem(taskWidget));
        delete taskWidget;
    }

    if (deleteSomething) {
        Q_EMIT taskModified(true);
        changeCurrentSelection();
    }

    logDebug() << "End of deletion";
}

void TasksWidget::refreshTasks(CTCron *cron)
{
    // Remove previous items
    removeAll();

    refreshHeaders();

    // Add new items
    const auto tasks = cron->tasks();
    for (CTTask *ctTask : tasks) {
        new TaskWidget(this, ctTask);
    }

    resizeColumnContents();
}

void TasksWidget::refreshHeaders()
{
    QStringList headerLabels;

    if (needUserColumn()) {
        headerLabels << i18n("User");
    }

    headerLabels << i18n("Scheduling");

    headerLabels << i18n("Command");
    headerLabels << i18n("Status");
    headerLabels << i18n("Description");
    headerLabels << i18n("Scheduling Details");

    treeWidget()->setHeaderLabels(headerLabels);

    if (needUserColumn()) {
        treeWidget()->setColumnCount(6);
    } else {
        treeWidget()->setColumnCount(5);
    }
}

bool TasksWidget::needUserColumn() const
{
    CTCron *ctCron = crontabWidget()->currentCron();
    if (ctCron && ctCron->isMultiUserCron()) {
        return true;
    }

    return false;
}

void TasksWidget::setupActions(CrontabWidget *crontabWidget)
{
    mNewTaskAction = new QAction(this);
    mNewTaskAction->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    mNewTaskAction->setText(i18nc("Adds a new task", "New &Task..."));
    mNewTaskAction->setToolTip(i18n("Create a new task."));
    addRightAction(mNewTaskAction, this, SLOT(createTask()));

    mModifyAction = new QAction(this);
    mModifyAction->setText(i18n("M&odify..."));
    mModifyAction->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    mModifyAction->setToolTip(i18n("Modify the selected task."));
    addRightAction(mModifyAction, this, SLOT(modifySelection()));

    mDeleteAction = new QAction(this);
    mDeleteAction->setText(i18n("&Delete"));
    mDeleteAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    mDeleteAction->setToolTip(i18n("Delete the selected task."));
    addRightAction(mDeleteAction, this, SLOT(deleteSelection()));

    mRunNowAction = new QAction(this);
    mRunNowAction->setText(i18n("&Run Now"));
    mRunNowAction->setIcon(QIcon::fromTheme(QStringLiteral("system-run")));
    mRunNowAction->setToolTip(i18n("Run the selected task now."));
    addRightAction(mRunNowAction, this, SLOT(runTaskNow()));

    mPrintAction = KStandardAction::print(crontabWidget, SLOT(print()), this);
    addRightAction(mPrintAction, crontabWidget, SLOT(print()));

    addRightStretch();
}

void TasksWidget::prepareContextualMenu()
{
    treeWidget()->addAction(mNewTaskAction);

    treeWidget()->addAction(createSeparator());

    treeWidget()->addAction(mModifyAction);
    treeWidget()->addAction(mDeleteAction);

    treeWidget()->addAction(createSeparator());
    const auto cutCopyPasteActions = crontabWidget()->cutCopyPasteActions();

    for (QAction *action : cutCopyPasteActions) {
        treeWidget()->addAction(action);
    }

    treeWidget()->addAction(createSeparator());

    treeWidget()->addAction(mRunNowAction);
}

void TasksWidget::toggleRunNowAction(bool state)
{
    setActionEnabled(mRunNowAction, state);
}

void TasksWidget::togglePrintAction(bool state)
{
    setActionEnabled(mPrintAction, state);
}

void TasksWidget::toggleModificationActions(bool state)
{
    setActionEnabled(mModifyAction, state);
    setActionEnabled(mDeleteAction, state);
}

void TasksWidget::toggleNewEntryAction(bool state)
{
    setActionEnabled(mNewTaskAction, state);
}

void TasksWidget::changeCurrentSelection()
{
    // logDebug() << "Change selection...";

    if (treeWidget()->topLevelItemCount() == 0) {
        togglePrintAction(false);
    } else {
        togglePrintAction(true);
    }

    bool enabled;
    if (treeWidget()->selectedItems().isEmpty()) {
        enabled = false;
    } else {
        enabled = true;
    }

    toggleModificationActions(enabled);
    toggleRunNowAction(enabled);
}
