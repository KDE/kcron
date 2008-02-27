/***************************************************************************
 *   KT list view item task header.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

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
