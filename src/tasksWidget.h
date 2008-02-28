/***************************************************************************
 *   KT list view item cron tasks folder.                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef TASKS_WIDGET_H
#define TASKS_WIDGET_H

#include <QTreeWidget>

#include "genericListWidget.h"
#include "cthost.h"

class TaskWidget;

/**
 * QTreeWidget of a "tasks" folder.
 */
class TasksWidget : public GenericListWidget {
	Q_OBJECT

public:

	/**
	 * Construct tasks folder from branch.
	 */
	TasksWidget(CrontabWidget* crontabWidget);
	
	~TasksWidget();

	TaskWidget* firstSelectedTaskWidget() const;
	
	QList<TaskWidget*> selectedTasksWidget() const;
	
	void refreshTasks(CTCron* cron);
	
	bool needUserColumn() const;

public slots:
	void modifySelection();
	
	void deleteSelection();

	/**
	 * Run task now.
	 */
	void runTaskNow() const;

	/**
	 * Create a new task.  Default is which type is most recently selected.
	 */
	void createTask();
	
	void addTask(CTTask* task);

protected slots:
	void modifySelection(QTreeWidgetItem* item, int position);
	
private:
	void refreshHeaders();
	
	int statusColumnIndex();

	
};

#endif // TASKS_WIDGET_H
