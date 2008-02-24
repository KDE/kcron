/***************************************************************************
 *   KT list view item tasks implementation.                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "variablesWidget.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QTreeWidgetItemIterator>

#include <klocale.h>
#include <kglobalsettings.h>

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "kticon.h"
#include "crontabWidget.h"
#include "variableWidget.h"
#include "variableEditorDialog.h"
#include "crontabPrinter.h"

/**
 * Construct tasks folder from branch.
 */
VariablesWidget::VariablesWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Environment Variables</b>"), KTIcon::variable(KTIcon::Small)) {
	
	QStringList headerLabels;

	/*
	if (crontabWidget()->isAllUsersSelected()) {
		headerLabels << i18n("Users");
	}
	*/

	headerLabels << i18n("Variable");
	headerLabels << i18n("Value");
	headerLabels << i18n("Status");
	headerLabels << i18n("Comment");

	treeWidget()->setHeaderLabels(headerLabels);
	treeWidget()->sortItems(0, Qt::AscendingOrder);

}

void VariablesWidget::modifySelection() {
	modifySelection(firstSelectedVariableWidget(), -1);
}

void VariablesWidget::modifySelection(QTreeWidgetItem* item, int position) {
	VariableWidget* variableWidget = static_cast<VariableWidget*>(item);
	
	if (variableWidget!=NULL) {
		
		if (position == statusColumnIndex()) {
			variableWidget->toggleEnable();
		}
		else {
			variableWidget->modify();
		}
	}
		
}


VariableWidget* VariablesWidget::firstSelectedVariableWidget() const {
	QTreeWidgetItem* item = firstSelected();
	if (item==NULL)
		return NULL;
	
	return static_cast<VariableWidget*>(item);

}

void VariablesWidget::deleteSelection() {
	QList<QTreeWidgetItem*> variablesItems = treeWidget()->selectedItems();
	foreach(QTreeWidgetItem* item, variablesItems) {
		VariableWidget* variableWidget = static_cast<VariableWidget*>(item);

		crontabWidget()->currentCron()->variables().removeAll(variableWidget->getCTVariable());
		delete variableWidget->getCTVariable();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(variableWidget) );
		delete variableWidget;
		
	}

}


void VariablesWidget::print(CrontabPrinter& printer) {
	QFont stnd = printer.getFont() ;
	printer.setFont(QFont(KGlobalSettings::generalFont().family(), 10, QFont::Bold));

	printer.print(i18nc("The environmental variable name ie HOME, MAILTO etc", "Variable:"), 1, CrontabPrinter::alignTextLeft);
	printer.print(i18n("Value:"), 2, CrontabPrinter::alignTextCenter);
	printer.print(i18n("Description:"), 3, CrontabPrinter::alignTextRight);

	printer.setFont(stnd);

	if (treeWidget()->topLevelItemCount() ==0) {
		printer.print(i18n("No variables..."), 1, CrontabPrinter::alignTextLeft, false);
		printer.levelColumns(20);
		return;
	}

	QTreeWidgetItemIterator it(treeWidget());
	while (*it != NULL) {
		VariableWidget* item = static_cast<VariableWidget*>(*it);

		item->print(printer);
		printer.levelColumns();

		++it;
	}

	printer.levelColumns(20);
}


int VariablesWidget::statusColumnIndex() {
	/*
	if (crontabWidget()->isAllUsersSelected())
		return 3;
	*/

	return 2;
}


void VariablesWidget::createVariable() {
	CTVariable* variable = new CTVariable();

	VariableEditorDialog variableEditorDialog(variable, i18n("New Variable"));
	variableEditorDialog.exec();

	if (variable->dirty()) {
		crontabWidget()->currentCron()->variables().append(variable);
		new VariableWidget(this, variable);
	} else {
		delete variable;
	}
}

void VariablesWidget::refreshVariables(CTCron* cron) {
	//Remove previous items
	removeAll();

	foreach(CTVariable* ctVariable, cron->variables()) {
		new VariableWidget(this, ctVariable);
	}
	
	resizeColumnContents();
}
