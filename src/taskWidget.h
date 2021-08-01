/*
    KT list view item task header.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QTreeWidgetItem>

class CTTask;
class TasksWidget;

/**
 * QTreeWidgetItem with a CTTask.
 */
class TaskWidget : public QTreeWidgetItem
{
public:
    /**
     * Initialize the list view item and task.
     */
    explicit TaskWidget(TasksWidget *tasksWidget, CTTask *_cttask);

    /*
     * Change the status of this task
     */
    void toggleEnable();

    /**
     * Get the task.
     */
    CTTask *getCTTask() const;

    /**
     * Refresh from underlying task.
     */
    void refresh();

private:
    /**
     * Task.
     */
    CTTask *mCtTask = nullptr;

    TasksWidget *mTasksWidget = nullptr;
};

