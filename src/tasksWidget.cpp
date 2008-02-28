/***************************************************************************
 *   KT list view item tasks implementation.                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "tasksWidget.h"

#include <QHeaderView>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QFile>
#include <QProcess>

#include <klocale.h>
#include <kglobalsettings.h>

#include "ctcron.h"
#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "kcronIcons.h"
#include "crontabWidget.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"

#include "logging.h"

/**
 * Construct tasks folder from branch.
 */
TasksWidget::TasksWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Scheduled Tasks</b>"), KCronIcons::task(KCronIcons::Small)) {
	
	refreshHeaders();

	treeWidget()->sortItems(1, Qt::AscendingOrder);

	logDebug() << "Tasks list created" << endl;

}

TasksWidget::~TasksWidget() {

}

QList<TaskWidget*> TasksWidget::selectedTasksWidget() const {
	QList<TaskWidget*> tasksWidget;
	
	QList<QTreeWidgetItem*> tasksItems = treeWidget()->selectedItems();
	foreach(QTreeWidgetItem* item, tasksItems) {
		TaskWidget* taskWidget = static_cast<TaskWidget*>(item);
		tasksWidget.append(taskWidget);
	}
	
	return tasksWidget;
}

TaskWidget* TasksWidget::firstSelectedTaskWidget() const {
	QTreeWidgetItem* item = firstSelected();
	if (item==NULL)
		return NULL;
	
	return static_cast<TaskWidget*>(item);
}

int TasksWidget::statusColumnIndex() {
	if (needUserColumn()) {
		return 3;
	}

	return 2;
}

void TasksWidget::runTaskNow() const {
	TaskWidget* taskWidget = firstSelectedTaskWidget();
	if (taskWidget == NULL)
		return;
	QString taskCommand = taskWidget->getCTTask()->command;
	
	
	QString echoMessage = i18nc("Do not use any quote characters (') in this string", "End of script execution. Type Enter or Ctrl+C to exit.");

	CTCron* ctCron = crontabWidget()->currentCron();
	if (ctCron == NULL) {
		logDebug() << "Unable to find the related CtCron, please report this bug to KCron developer" << endl;
		return;
	}
	

	QStringList commandList;
	
	foreach(CTVariable* variable, ctCron->variables()) {
		commandList << QString("export %1=\"%2\"").arg(variable->variable, variable->value);
	}
	
	commandList << taskCommand;
	commandList << "echo '-------------------------------------------------------------------------'";
	commandList << "echo " + echoMessage;
	commandList << "echo '-------------------------------------------------------------------------'";
	commandList << "read";
	
	QStringList parameters;
	parameters << "-e" << "bash" << "-c";
	parameters << commandList.join(";");
	
	QProcess process;
	process.startDetached("konsole", parameters);

}

void TasksWidget::createTask() {
	CTTask* task = new CTTask("", "", crontabWidget()->currentCron()->userLogin(), crontabWidget()->currentCron()->isMultiUserCron());

	TaskEditorDialog taskEditorDialog(task, i18n("New Task"), crontabWidget());
	taskEditorDialog.exec();

	if (task->dirty()) {
		addTask(task);
	}
	else {
		delete task;
	}
}

void TasksWidget::addTask(CTTask* task) {
	CTCron* cron = crontabWidget()->currentCron();
	
	cron->addTask(task);
	new TaskWidget(this, task);
}

void TasksWidget::modifySelection() {
	modifySelection(firstSelectedTaskWidget(), -1);
}

void TasksWidget::modifySelection(QTreeWidgetItem* item, int position) {
	TaskWidget* taskWidget = static_cast<TaskWidget*>(item);
	if (taskWidget!=NULL) {
		
		if (position == statusColumnIndex()) {
			taskWidget->toggleEnable();
		}
		else {
			CTTask* task = taskWidget->getCTTask();
			TaskEditorDialog taskEditorDialog(task, i18n("Modify Task"), crontabWidget());
			taskEditorDialog.exec();
			
			crontabWidget()->currentCron()->modifyTask(task);
			taskWidget->refresh();
		}

	}
	
	logDebug() << "End of modification" << endl;
		
}

void TasksWidget::deleteSelection() {
	logDebug() << "Selection deleting..." << endl;
	
	QList<QTreeWidgetItem*> tasksItems = treeWidget()->selectedItems();
	foreach(QTreeWidgetItem* item, tasksItems) {
		TaskWidget* taskWidget = static_cast<TaskWidget*>(item);

		crontabWidget()->currentCron()->removeTask(taskWidget->getCTTask());
		delete taskWidget->getCTTask();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(taskWidget) );
		delete taskWidget;
		
	}

	logDebug() << "End of deletion" << endl;
}

void TasksWidget::refreshTasks(CTCron* cron) {
	//Remove previous items
	removeAll();
	
	refreshHeaders();
	
	//Add new items
	foreach(CTTask* ctTask, cron->tasks()) {
		new TaskWidget(this, ctTask);
	}
	
	resizeColumnContents();

}

void TasksWidget::refreshHeaders() {
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
	
	if (needUserColumn())
		treeWidget()->setColumnCount(6);
	else
		treeWidget()->setColumnCount(5);

}

bool TasksWidget::needUserColumn() const {
	if (crontabWidget()->currentCron()->isMultiUserCron()) {
		return true;
	}
	
	return false;
	
}
