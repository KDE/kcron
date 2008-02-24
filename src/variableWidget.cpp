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
#include "kticon.h"
#include "taskEditorDialog.h"
#include "crontabPrinter.h"

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
		qpIcon = KTIcon::mail(KTIcon::Small);
	else if (ctVariable->variable == "SHELL")
		qpIcon = KTIcon::shell(KTIcon::Small);
	else if (ctVariable->variable == "HOME")
		qpIcon = KTIcon::home(KTIcon::Small);
	else if (ctVariable->variable == "PATH")
		qpIcon = KTIcon::path(KTIcon::Small);
	else
		qpIcon = KTIcon::variable(KTIcon::Small);

	setIcon(0, QIcon(qpIcon));
}

void VariableWidget::print(CrontabPrinter& printer) const {
	printer.print(ctVariable->variable, 1, CrontabPrinter::alignTextLeft);
	if (ctVariable->enabled) {
		printer.print(ctVariable->value, 2, CrontabPrinter::alignTextCenter);
		printer.print(ctVariable->comment, 3, CrontabPrinter::alignTextRight);
	} else {
		printer.print(i18nc("The cron variable has been disabled", "Disabled."), 3, CrontabPrinter::alignTextRight);
	}

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
