/*
    KT list view item task implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "taskWidget.h"

#include <KLocalizedString>

#include "cttask.h"
#include "ctcron.h"

#include "crontabWidget.h"
#include "taskEditorDialog.h"

#include "logging.h"

TaskWidget::TaskWidget(TasksWidget *_tasksWidget, CTTask *_cttask)
    : QTreeWidgetItem(_tasksWidget->treeWidget())
{
    ctTask = _cttask;
    tasksWidget = _tasksWidget;

    refresh();
}

void TaskWidget::refresh()
{
    int column = 0;

    if (tasksWidget->needUserColumn()) {
        setText(column++, ctTask->userLogin);
    }

    setText(column++, ctTask->schedulingCronFormat());

    setText(column, ctTask->command);
    setIcon(column++, ctTask->commandIcon());

    if (ctTask->enabled) {
        setText(column, i18n("Enabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    } else {
        setText(column, i18n("Disabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-cancel")));
    }

    setText(column++, ctTask->comment);
    setText(column++, ctTask->describe());
}

void TaskWidget::toggleEnable()
{
    ctTask->enabled = !ctTask->enabled;
    refresh();
}

CTTask *TaskWidget::getCTTask() const
{
    return ctTask;
}
