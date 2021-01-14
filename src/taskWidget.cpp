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
    mCtTask = _cttask;
    mTasksWidget = _tasksWidget;

    refresh();
}

void TaskWidget::refresh()
{
    int column = 0;

    if (mTasksWidget->needUserColumn()) {
        setText(column++, mCtTask->userLogin);
    }

    setText(column++, mCtTask->schedulingCronFormat());

    setText(column, mCtTask->command);
    setIcon(column++, mCtTask->commandIcon());

    if (mCtTask->enabled) {
        setText(column, i18n("Enabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    } else {
        setText(column, i18n("Disabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-cancel")));
    }

    setText(column++, mCtTask->comment);
    setText(column++, mCtTask->describe());
}

void TaskWidget::toggleEnable()
{
    mCtTask->enabled = !mCtTask->enabled;
    refresh();
}

CTTask *TaskWidget::getCTTask() const
{
    return mCtTask;
}
