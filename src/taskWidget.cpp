/***************************************************************************
 *   KT list view item task implementation.                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "taskWidget.h"

#include <klocale.h>
#include <kiconloader.h>

#include "cttask.h"
#include "ctcron.h"

#include "tasksWidget.h"
#include "crontabWidget.h"
#include "kcronIcons.h"
#include "taskEditorDialog.h"

#include "logging.h"

TaskWidget::TaskWidget(TasksWidget* _tasksWidget, CTTask* _cttask) :
	QTreeWidgetItem(_tasksWidget->treeWidget()) {

	ctTask = _cttask;
	tasksWidget = _tasksWidget;

	refresh();
}

void TaskWidget::refresh() {
	int column = 0;
	
	if (tasksWidget->needUserColumn()) {
		setText(column++, ctTask->userLogin);
	}
	
	setText(column++, ctTask->schedulingCronFormat());
	
	setText(column, ctTask->command);
	setIcon(column++, ctTask->commandIcon());

	if (ctTask->enabled) {
		setText(column, i18n("Enabled"));
		setIcon(column++, SmallIcon("ok"));
	} else {
		setText(column, i18n("Disabled"));
		setIcon(column++, SmallIcon("no"));
	}

	setText(column++, ctTask->comment);
	setText(column++, ctTask->describe());

}

void TaskWidget::toggleEnable() {
	ctTask->enabled = !ctTask->enabled;
	refresh();
}

CTTask* TaskWidget::getCTTask() const {
	return ctTask;
}
