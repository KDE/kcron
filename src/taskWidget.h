/*
    KT list view item task header.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TASK_WIDGET_H
#define TASK_WIDGET_H

#include <QTreeWidgetItem>

class CTTask;
class TasksWidget;

/**
 * QTreeWidgetItem with a CTTask.
 */
class TaskWidget : public QTreeWidgetItem {
public:

	/**
	 * Initialize the list view item and task.
	 */
	TaskWidget(TasksWidget* tasksWidget, CTTask* _cttask);

	/*
	 * Change the status of this task
	 */
	void toggleEnable();

	/**
	 * Get the task.
	 */
	CTTask* getCTTask() const;

	/**
	 * Refresh from underlying task.
	 */
	void refresh();

private:
	
	/**
	 * Task.
	 */
	CTTask* ctTask;
	
	TasksWidget* tasksWidget;
};

#endif // TASK_WIDGET_H
