/*
    KT list view item cron tasks folder.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TASKS_WIDGET_H
#define TASKS_WIDGET_H

#include <QTreeWidget>

#include "cthost.h"
#include "genericListWidget.h"

class TaskWidget;

/**
 * QTreeWidget of a "tasks" folder.
 */
class TasksWidget : public GenericListWidget
{
    Q_OBJECT

public:
    /**
     * Construct tasks folder from branch.
     */
    explicit TasksWidget(CrontabWidget *crontabWidget);

    ~TasksWidget() override;

    TaskWidget *firstSelectedTaskWidget() const;

    QList<TaskWidget *> selectedTasksWidget() const;

    void refreshTasks(CTCron *cron);

    bool needUserColumn() const;

    /**
     * Enables/disables modification buttons
     */
    void toggleModificationActions(bool enabled);

    /**
     * Enables/disables new entry actions
     */
    void toggleNewEntryAction(bool enabled);

    /**
     * Enables/disables "Run now"
     */
    void toggleRunNowAction(bool enabled);

    /**
     * Enables/disables Print Action
     */
    void togglePrintAction(bool enabled);

Q_SIGNALS:
    void taskModified(bool);

public Q_SLOTS:
    void modifySelection();

    void deleteSelection() override;

    /**
     * Run task now.
     */
    void runTaskNow() const;

    /**
     * Create a new task.  Default is which type is most recently selected.
     */
    void createTask();

    void addTask(CTTask *task);

    void changeCurrentSelection();

protected Q_SLOTS:
    void modifySelection(QTreeWidgetItem *item, int position) override;

private:
    void refreshHeaders();

    int statusColumnIndex();

    void setupActions(CrontabWidget *crontabWidget);
    void prepareContextualMenu();

    QAction *mNewTaskAction = nullptr;

    QAction *mModifyAction = nullptr;

    QAction *mDeleteAction = nullptr;

    QAction *mRunNowAction = nullptr;

    QAction *mPrintAction = nullptr;
};

#endif // TASKS_WIDGET_H
