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

#include <QProcess>
#include <QAction>
#include <QList>

#include <kstandardaction.h>
#include <KLocalizedString>

#include "ctcron.h"
#include "cthost.h"
#include "cttask.h"
#include "ctvariable.h"

#include "crontabWidget.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"

#include "logging.h"

class TasksWidgetPrivate {
public:

	QAction* newTaskAction;

	QAction* modifyAction;

	QAction* deleteAction;

	QAction* runNowAction;

	QAction* printAction;

};

/**
 * Construct tasks folder from branch.
 */
TasksWidget::TasksWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Scheduled Tasks</b>"), QIcon::fromTheme(QLatin1String("system-run"))),
	d(new TasksWidgetPrivate()) {

	refreshHeaders();

	treeWidget()->sortItems(1, Qt::AscendingOrder);

	setupActions(crontabWidget);
	prepareContextualMenu();

	connect(treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(changeCurrentSelection()));

	logDebug() << "Tasks list created" << endl;
}

TasksWidget::~TasksWidget() {
	delete d;
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
            commandList << QString::fromLatin1("export %1=\"%2\"").arg(variable->variable, variable->value);
	}

	commandList << taskCommand;
	commandList << QLatin1String( "echo '-------------------------------------------------------------------------'" );
	commandList << QLatin1String( "echo " ) + echoMessage;
	commandList << QLatin1String( "echo '-------------------------------------------------------------------------'" );
	commandList << QLatin1String( "read" );

	QStringList parameters;
	parameters << QLatin1String( "-e" ) << QLatin1String( "bash" ) << QLatin1String( "-c" );
	parameters << commandList.join(QLatin1String( ";" ));

	QProcess process;
	process.startDetached(QLatin1String( "konsole" ), parameters);

}

void TasksWidget::createTask() {
	CTTask* task = new CTTask(QLatin1String( "" ), QLatin1String( "" ), crontabWidget()->currentCron()->userLogin(), crontabWidget()->currentCron()->isMultiUserCron());

	TaskEditorDialog taskEditorDialog(task, i18n("New Task"), crontabWidget());
	int result = taskEditorDialog.exec();

	if (result == QDialog::Accepted) {
		addTask(task);
		emit taskModified(true);

		changeCurrentSelection();
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
			emit taskModified(true);
		}
		else {
			CTTask* task = taskWidget->getCTTask();
			TaskEditorDialog taskEditorDialog(task, i18n("Modify Task"), crontabWidget());
			int result = taskEditorDialog.exec();

			if (result == QDialog::Accepted) {
				crontabWidget()->currentCron()->modifyTask(task);
				taskWidget->refresh();
				emit taskModified(true);
			}
		}

	}

	logDebug() << "End of modification" << endl;

}

void TasksWidget::deleteSelection() {
	logDebug() << "Selection deleting..." << endl;

	QList<QTreeWidgetItem*> tasksItems = treeWidget()->selectedItems();

	bool deleteSomething = ! (tasksItems.isEmpty());

	foreach(QTreeWidgetItem* item, tasksItems) {
		TaskWidget* taskWidget = static_cast<TaskWidget*>(item);

		crontabWidget()->currentCron()->removeTask(taskWidget->getCTTask());
		delete taskWidget->getCTTask();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(taskWidget) );
		delete taskWidget;

	}

	if (deleteSomething) {
		emit taskModified(true);
		changeCurrentSelection();
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
	CTCron* ctCron = crontabWidget()->currentCron();
	if (ctCron && ctCron->isMultiUserCron()) {
		return true;
	}

	return false;

}

void TasksWidget::setupActions(CrontabWidget* crontabWidget) {

	d->newTaskAction = new QAction(this);
	d->newTaskAction->setIcon(QIcon::fromTheme( QLatin1String( "document-new" )));
	d->newTaskAction->setText(i18nc("Adds a new task", "New &Task...") );
	d->newTaskAction->setToolTip(i18n("Create a new task."));
	addRightAction(d->newTaskAction, this, SLOT(createTask()));

	d->modifyAction = new QAction(this);
	d->modifyAction->setText(i18n("M&odify...") );
	d->modifyAction->setIcon(QIcon::fromTheme( QLatin1String( "document-open" )) );
	d->modifyAction->setToolTip(i18n("Modify the selected task."));
	addRightAction(d->modifyAction, this, SLOT(modifySelection()));

	d->deleteAction = new QAction(this);
	d->deleteAction->setText(i18n("&Delete") );
	d->deleteAction->setIcon(QIcon::fromTheme( QLatin1String( "edit-delete" )) );
	d->deleteAction->setToolTip(i18n("Delete the selected task."));
	addRightAction(d->deleteAction, this, SLOT(deleteSelection()));

	d->runNowAction = new QAction(this);
	d->runNowAction->setText(i18n("&Run Now") );
	d->runNowAction->setIcon(QIcon::fromTheme( QLatin1String( "system-run" )));
	d->runNowAction->setToolTip(i18n("Run the selected task now."));
	addRightAction(d->runNowAction, this, SLOT(runTaskNow()));

	d->printAction = KStandardAction::print(crontabWidget, SLOT(print()), this);
	addRightAction(d->printAction, crontabWidget, SLOT(print()));

	addRightStretch();
}

void TasksWidget::prepareContextualMenu() {

	treeWidget()->addAction(d->newTaskAction);

	treeWidget()->addAction(createSeparator());

	treeWidget()->addAction(d->modifyAction);
	treeWidget()->addAction(d->deleteAction);

	treeWidget()->addAction(createSeparator());

	foreach(QAction* action, crontabWidget()->cutCopyPasteActions()) {
		treeWidget()->addAction(action);
	}

	treeWidget()->addAction(createSeparator());

	treeWidget()->addAction(d->runNowAction);

}

void TasksWidget::toggleRunNowAction(bool state) {
	setActionEnabled(d->runNowAction, state);
}

void TasksWidget::togglePrintAction(bool state) {
	setActionEnabled(d->printAction, state);
}

void TasksWidget::toggleModificationActions(bool state) {
	setActionEnabled(d->modifyAction, state);
	setActionEnabled(d->deleteAction, state);
}

void TasksWidget::toggleNewEntryAction(bool state) {
	setActionEnabled(d->newTaskAction, state);
}

void TasksWidget::changeCurrentSelection() {
	//logDebug() << "Change selection..." << endl;

	if (treeWidget()->topLevelItemCount()==0) {
		togglePrintAction(false);
	}
	else {
		togglePrintAction(true);
	}

	bool enabled;
	if (treeWidget()->selectedItems().isEmpty())
		enabled = false;
	else
		enabled = true;

	toggleModificationActions(enabled);
	toggleRunNowAction(enabled);

}
