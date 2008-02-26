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

#include "kcronIcons.h"
#include "crontabWidget.h"
#include "variableWidget.h"
#include "variableEditorDialog.h"

#include "logging.h"

/**
 * Construct tasks folder from branch.
 */
VariablesWidget::VariablesWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Environment Variables</b>"), KCronIcons::variable(KCronIcons::Small)) {
	
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

		crontabWidget()->currentCron()->removeVariable(variableWidget->getCTVariable());
		delete variableWidget->getCTVariable();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(variableWidget) );
		delete variableWidget;
		
	}

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
		logDebug() << "Add a new variable" << endl;
		crontabWidget()->currentCron()->addVariable(variable);
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
