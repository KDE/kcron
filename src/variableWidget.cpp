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

#include "ctvariable.h"

#include "variableEditorDialog.h"
#include "kcronIcons.h"
#include "taskEditorDialog.h"

VariableWidget::VariableWidget(VariablesWidget* variablesWidget, CTVariable* _ctVariable) :
	QTreeWidgetItem(variablesWidget->treeWidget()) {

	ctVariable = _ctVariable;

	refresh();
}

void VariableWidget::refresh() {
	setText(0, ctVariable->variable);
	setText(1, ctVariable->value);

	if (ctVariable->enabled) {
		setText(2, i18n("Enabled"));
		setIcon(2, SmallIcon("ok"));
	}
	else {
		setText(2, i18n("Disabled"));
		setIcon(2, SmallIcon("no"));
	}
	
	setText(3, ctVariable->comment);

	QPixmap qpIcon;
	if (ctVariable->variable == "MAILTO")
		qpIcon = KCronIcons::mail(KCronIcons::Small);
	else if (ctVariable->variable == "SHELL")
		qpIcon = KCronIcons::shell(KCronIcons::Small);
	else if (ctVariable->variable == "HOME")
		qpIcon = KCronIcons::home(KCronIcons::Small);
	else if (ctVariable->variable == "PATH")
		qpIcon = KCronIcons::path(KCronIcons::Small);
	else
		qpIcon = KCronIcons::variable(KCronIcons::Small);

	setIcon(0, QIcon(qpIcon));
}

void VariableWidget::modify() {
	VariableEditorDialog(ctVariable, i18n("Modify Variable")).exec();
	refresh();
}

void VariableWidget::toggleEnable() {
	ctVariable->enabled = ! ctVariable->enabled;
	refresh();
}

CTVariable* VariableWidget::getCTVariable() const {
	return ctVariable;
}
