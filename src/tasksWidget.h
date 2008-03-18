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

class TasksWidgetPrivate;

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

signals:
	void taskModified(bool);

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

	void changeCurrentSelection();

protected slots:
	void modifySelection(QTreeWidgetItem* item, int position);

private:
	void refreshHeaders();
	
	int statusColumnIndex();

	void setupActions(CrontabWidget* crontabWidget);
	void prepareContextualMenu();
	

	TasksWidgetPrivate* const d;
};

#endif // TASKS_WIDGET_H
