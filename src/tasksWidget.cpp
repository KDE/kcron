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

#include "kticon.h"
#include "crontabWidget.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"
#include "crontabPrinter.h"

#include "logging.h"

/**
 * Construct tasks folder from branch.
 */
TasksWidget::TasksWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Tasks</b>"), KTIcon::task(KTIcon::Small)) {
	
	QStringList headerLabels;
	
	/*
	if (crontabWidget()->isAllUsersSelected()) {
		headerLabels << i18n("Users");
	}
	*/

	headerLabels << i18n("Scheduling");
	headerLabels << i18n("Command");
	headerLabels << i18n("Status");
	headerLabels << i18n("Description");
	headerLabels << i18n("Scheduling Details");

	treeWidget()->setHeaderLabels(headerLabels);

	treeWidget()->sortItems(1, Qt::AscendingOrder);

	logDebug() << "Tasks list created" << endl;

}

TasksWidget::~TasksWidget() {

}

void TasksWidget::print(CrontabPrinter& printer) {
	QFont stnd = printer.getFont();

	printer.setFont(QFont(KGlobalSettings::generalFont().family(), 10, QFont::Bold));
	printer.print(i18n("Task name:"), 1, CrontabPrinter::alignTextLeft);
	printer.print(i18n("Program:"), 2, CrontabPrinter::alignTextCenter);
	printer.print(i18n("Description:"), 3, CrontabPrinter::alignTextRight);

	printer.setFont(stnd);

	if (treeWidget()->topLevelItemCount() == 0) {
		printer.print(i18n("No tasks..."), 1, CrontabPrinter::alignTextLeft, false);
		printer.levelColumns(20);
		return;
	}

	QTreeWidgetItemIterator it(treeWidget());
	while (*it != NULL) {
		TaskWidget* item = static_cast<TaskWidget*>(*it);
		
		item->print(printer);
		printer.levelColumns();
			
		++it;
	}

	printer.levelColumns(20);
}


TaskWidget* TasksWidget::firstSelectedTaskWidget() const {
	QTreeWidgetItem* item = firstSelected();
	if (item==NULL)
		return NULL;
	
	return static_cast<TaskWidget*>(item);
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
			taskWidget->modify();
		}

	}
	
	logDebug() << "End of modification" << endl;
		
}

void TasksWidget::deleteSelection() {

	QList<QTreeWidgetItem*> tasksItems = treeWidget()->selectedItems();
	foreach(QTreeWidgetItem* item, tasksItems) {
		TaskWidget* taskWidget = static_cast<TaskWidget*>(item);

		crontabWidget()->currentCron()->tasks().removeAll(taskWidget->getCTTask());
		delete taskWidget->getCTTask();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(taskWidget) );
		delete taskWidget;
		
	}

	logDebug() << "End of deletion" << endl;
}

int TasksWidget::statusColumnIndex() {
	/*
	if (crontabWidget()->isAllUsersSelected())
		return 3;
	*/

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
	CTTask* task = new CTTask("", "", crontabWidget()->currentCron()->isSystemCron());

	TaskEditorDialog taskEditorDialog(task, i18n("New Task"));
	taskEditorDialog.exec();

	if (task->dirty()) {
		crontabWidget()->currentCron()->tasks().append(task);
		new TaskWidget(this, task);
	} else {
		delete task;
	}
}


void TasksWidget::refreshTasks(CTCron* cron) {
	//Remove previous items
	removeAll();
	
	//Add new items
	foreach(CTTask* ctTask, cron->tasks()) {
		new TaskWidget(this, ctTask);
	}
	
	resizeColumnContents();

}
