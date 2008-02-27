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

#include "cttask.h"
#include "ctcron.h"

#include "tasksWidget.h"
#include "crontabWidget.h"
#include "kcronIcons.h"
#include "taskEditorDialog.h"
#include "kiconloader.h"

TaskWidget::TaskWidget(TasksWidget* _tasksWidget, CTTask* _cttask) :
	QTreeWidgetItem(_tasksWidget->treeWidget()) {

	ctTask = _cttask;
	tasksWidget = _tasksWidget;

	refresh();
}

void TaskWidget::refresh() {
	int column = 0;
	
	setText(column++, ctTask->schedulingCronFormat());
	
	if (tasksWidget->crontabWidget()->currentCron()->isSystemCron()) {
		setText(column++, ctTask->user);
	}
	
	setText(column++, ctTask->command);

	if (ctTask->enabled) {
		setText(column, i18n("Enabled"));
		setIcon(column++, SmallIcon("ok"));
	} else {
		setText(column, i18n("Disabled"));
		setIcon(column++, SmallIcon("no"));
	}

	setText(column++, ctTask->comment);
	setText(column++, ctTask->describe());

	QString qsCommand = ctTask->command;

	// qsCommand broken down this way to split off qsCommand attributes
	int firstSpace(qsCommand.indexOf(" "));
	if (firstSpace > 0)
		qsCommand = qsCommand.left(firstSpace);
	int lastSlash(qsCommand.lastIndexOf("/"));
	if (lastSlash > 0)
		qsCommand = qsCommand.right(qsCommand.size() - lastSlash - 1);

	QPixmap qpIcon = SmallIcon(qsCommand);
	if (qpIcon.isNull())
		qpIcon = KCronIcons::task(KCronIcons::Small);

	setIcon(0, QIcon(qpIcon));
}

void TaskWidget::toggleEnable() {
	ctTask->enabled = !ctTask->enabled;
	refresh();
}

CTTask* TaskWidget::getCTTask() const {
	return ctTask;
}
