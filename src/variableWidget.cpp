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

#include "variableWidget.h"

#include <klocale.h>
#include <kiconloader.h>

#include "ctvariable.h"

#include "variableEditorDialog.h"
#include "kcronIcons.h"
#include "taskEditorDialog.h"

VariableWidget::VariableWidget(VariablesWidget* _variablesWidget, CTVariable* _ctVariable) :
	QTreeWidgetItem(_variablesWidget->treeWidget()) {

	ctVariable = _ctVariable;
	variablesWidget = _variablesWidget;

	refresh();
}

void VariableWidget::refresh() {
	int column = 0;

	if (variablesWidget->needUserColumn()) {
		setText(column++, ctVariable->userLogin);
	}

	setText(column, ctVariable->variable);
	setIcon(column++, ctVariable->variableIcon());

	setText(column++, ctVariable->value);

	if (ctVariable->enabled) {
		setText(column, i18n("Enabled"));
		setIcon(column++, SmallIcon(QLatin1String( "dialog-ok-apply" )));
	}
	else {
		setText(column, i18n("Disabled"));
		setIcon(column++, SmallIcon(QLatin1String( "dialog-cancel" )));
	}

	setText(column++, ctVariable->comment);


}


void VariableWidget::toggleEnable() {
	ctVariable->enabled = ! ctVariable->enabled;
	refresh();
}

CTVariable* VariableWidget::getCTVariable() const {
	return ctVariable;
}
