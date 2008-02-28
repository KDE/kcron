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
	setIcon(column++, findIcon());

	setText(column++, ctVariable->value);

	if (ctVariable->enabled) {
		setText(column, i18n("Enabled"));
		setIcon(column++, SmallIcon("ok"));
	}
	else {
		setText(column, i18n("Disabled"));
		setIcon(column++, SmallIcon("no"));
	}
	
	setText(column++, ctVariable->comment);

			
}

QIcon VariableWidget::findIcon() const {
	
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

	return QIcon(qpIcon);
}

void VariableWidget::toggleEnable() {
	ctVariable->enabled = ! ctVariable->enabled;
	refresh();
}

CTVariable* VariableWidget::getCTVariable() const {
	return ctVariable;
}
