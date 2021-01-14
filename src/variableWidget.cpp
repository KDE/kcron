/*
    KT list view item task implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "variableWidget.h"

#include <KLocalizedString>

#include "ctvariable.h"

#include "variableEditorDialog.h"
#include "taskEditorDialog.h"

VariableWidget::VariableWidget(VariablesWidget *_variablesWidget, CTVariable *_ctVariable)
    : QTreeWidgetItem(_variablesWidget->treeWidget())
{
    ctVariable = _ctVariable;
    variablesWidget = _variablesWidget;

    refresh();
}

void VariableWidget::refresh()
{
    int column = 0;

    if (variablesWidget->needUserColumn()) {
        setText(column++, ctVariable->userLogin);
    }

    setText(column, ctVariable->variable);
    setIcon(column++, ctVariable->variableIcon());

    setText(column++, ctVariable->value);

    if (ctVariable->enabled) {
        setText(column, i18n("Enabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-ok-apply")));
    } else {
        setText(column, i18n("Disabled"));
        setIcon(column++, QIcon::fromTheme(QStringLiteral("dialog-cancel")));
    }

    setText(column++, ctVariable->comment);
}

void VariableWidget::toggleEnable()
{
    ctVariable->enabled = !ctVariable->enabled;
    refresh();
}

CTVariable *VariableWidget::getCTVariable() const
{
    return ctVariable;
}
