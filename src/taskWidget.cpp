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

#include "kticon.h"
#include "taskEditorDialog.h"
#include "ktprint.h"
#include "kiconloader.h"

TaskWidget::TaskWidget(TasksWidget* tasksWidget, CTTask* _cttask) :
	QTreeWidgetItem(tasksWidget->treeWidget()) {

	ctTask = _cttask;

	refresh();
}

void TaskWidget::refresh() {
	setText(0, ctTask->schedulingCronFormat());
	setText(1, ctTask->command);

	if (ctTask->enabled) {
		setText(2, i18n("Enabled"));
		setIcon(2, SmallIcon("ok"));
	} else {
		setText(2, i18n("Disabled"));
		setIcon(2, SmallIcon("no"));
	}

	setText(3, ctTask->comment);
	setText(4, ctTask->describe());

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
		qpIcon = KTIcon::task(KTIcon::Small);

	setIcon(0, QIcon(qpIcon));
}

void TaskWidget::print(KTPrint& printer) const {
	printer.print(ctTask->comment, 1, KTPrint::alignTextLeft);
	if (ctTask->enabled) {
		printer.print(ctTask->command, 2, KTPrint::alignTextCenter);

		if (ctTask->reboot)
			printer.print(i18n("At system startup"), 3, KTPrint::alignTextRight);
		else
			printer.print(ctTask->describe(), 3, KTPrint::alignTextRight);
	} else
		printer.print(i18nc("The cron task had been disabled", "Disabled."), 3, KTPrint::alignTextRight);
}

void TaskWidget::modify() {
	TaskEditorDialog(ctTask, i18n("Modify Task")).exec();
	refresh();
}

void TaskWidget::toggleEnable() {
	ctTask->enabled = !ctTask->enabled;
	refresh();
}

CTTask* TaskWidget::getCTTask() const {
	return ctTask;
}
