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
#include <QAction>
#include <QList>

#include <klocale.h>
#include <kglobalsettings.h>
#include <kicon.h>

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "kcronIcons.h"
#include "crontabWidget.h"
#include "variableWidget.h"
#include "variableEditorDialog.h"

#include "logging.h"

class VariablesWidgetPrivate {
public:

	QAction* newVariableAction;

	QAction* modifyAction;

	QAction* deleteAction;

};
/**
 * Construct tasks folder from branch.
 */
VariablesWidget::VariablesWidget(CrontabWidget* crontabWidget) :
	GenericListWidget(crontabWidget, i18n("<b>Environment Variables</b>"), KCronIcons::variable(KCronIcons::Small)),
	d(new VariablesWidgetPrivate()) {

	refreshHeaders();

	treeWidget()->sortItems(0, Qt::AscendingOrder);

	setupActions();
	prepareContextualMenu();

	connect(treeWidget(), SIGNAL(itemSelectionChanged()), this, SLOT(changeCurrentSelection()));

	logDebug() << "Variables list created" << endl;

}

VariablesWidget::~VariablesWidget() {
	delete d;
}

void VariablesWidget::modifySelection() {
	modifySelection(firstSelectedVariableWidget(), -1);
}

void VariablesWidget::modifySelection(QTreeWidgetItem* item, int position) {
	VariableWidget* variableWidget = static_cast<VariableWidget*>(item);

	if (variableWidget!=NULL) {

		if (position == statusColumnIndex()) {
			variableWidget->toggleEnable();
			emit variableModified(true);
		}
		else {
			CTVariable* variable = variableWidget->getCTVariable();
			VariableEditorDialog variableEditorDialog(variable, i18n("Modify Variable"), crontabWidget());
			int result = variableEditorDialog.exec();

			if (result == QDialog::Accepted) {
				crontabWidget()->currentCron()->modifyVariable(variable);
				variableWidget->refresh();

				emit variableModified(true);
			}
		}
	}

}

QList<VariableWidget*> VariablesWidget::selectedVariablesWidget() const {
	QList<VariableWidget*> variablesWidget;

	QList<QTreeWidgetItem*> variablesItems = treeWidget()->selectedItems();
	foreach(QTreeWidgetItem* item, variablesItems) {
		VariableWidget* variableWidget = static_cast<VariableWidget*>(item);
		variablesWidget.append(variableWidget);
	}

	return variablesWidget;
}


VariableWidget* VariablesWidget::firstSelectedVariableWidget() const {
	QTreeWidgetItem* item = firstSelected();
	if (item==NULL)
		return NULL;

	return static_cast<VariableWidget*>(item);

}

void VariablesWidget::deleteSelection() {
	QList<QTreeWidgetItem*> variablesItems = treeWidget()->selectedItems();
	bool deleteSomething = ! (variablesItems.isEmpty());

	foreach(QTreeWidgetItem* item, variablesItems) {
		VariableWidget* variableWidget = static_cast<VariableWidget*>(item);

		crontabWidget()->currentCron()->removeVariable(variableWidget->getCTVariable());
		delete variableWidget->getCTVariable();
		treeWidget()->takeTopLevelItem( treeWidget()->indexOfTopLevelItem(variableWidget) );
		delete variableWidget;

	}

	if (deleteSomething) {
		emit variableModified(true);
		changeCurrentSelection();
	}

}

bool VariablesWidget::needUserColumn() {
	CTCron* currentCron = crontabWidget()->currentCron();
	if (currentCron->isMultiUserCron()==true && currentCron->isSystemCron()==false)
		return true;

	return false;
}

int VariablesWidget::statusColumnIndex() {
	if (needUserColumn() == true)
		return 3;

	return 2;
}


void VariablesWidget::createVariable() {
	CTVariable* variable = new CTVariable(QLatin1String( "" ), QLatin1String( "" ), crontabWidget()->currentCron()->userLogin());

	VariableEditorDialog variableEditorDialog(variable, i18n("New Variable"), crontabWidget());
	int result = variableEditorDialog.exec();

	if (result == QDialog::Accepted) {
		addVariable(variable);
		emit variableModified(true);
		changeCurrentSelection();
	}
	else {
		delete variable;
	}
}

void VariablesWidget::addVariable(CTVariable* variable) {
	logDebug() << "Add a new variable" << endl;
	crontabWidget()->currentCron()->addVariable(variable);
	new VariableWidget(this, variable);

	changeCurrentSelection();
}

void VariablesWidget::refreshVariables(CTCron* cron) {
	//Remove previous items
	removeAll();

	refreshHeaders();

	foreach(CTVariable* ctVariable, cron->variables()) {
		new VariableWidget(this, ctVariable);
	}

	resizeColumnContents();
}

void VariablesWidget::refreshHeaders() {
	QStringList headerLabels;

	if (needUserColumn()) {
		headerLabels << i18n("User");
	}

	headerLabels << i18n("Variable");
	headerLabels << i18n("Value");
	headerLabels << i18n("Status");
	headerLabels << i18n("Comment");

	treeWidget()->setHeaderLabels(headerLabels);

	if (needUserColumn())
		treeWidget()->setColumnCount(5);
	else
		treeWidget()->setColumnCount(4);

}

void VariablesWidget::setupActions() {

	d->newVariableAction = new QAction(this);
	d->newVariableAction->setIcon(KIcon( QLatin1String( "document-new" )));
	d->newVariableAction->setText(i18nc("Adds a new variable", "New &Variable...") );
	d->newVariableAction->setToolTip(i18n("Create a new variable."));
	addRightAction(d->newVariableAction, this, SLOT(createVariable()));

	d->modifyAction = new QAction(this);
	d->modifyAction->setText(i18n("M&odify...") );
	d->modifyAction->setIcon(KIcon( QLatin1String( "document-open" )) );
	d->modifyAction->setToolTip(i18n("Modify the selected variable."));
	addRightAction(d->modifyAction, this, SLOT(modifySelection()));

	d->deleteAction = new QAction(this);
	d->deleteAction->setText(i18n("&Delete") );
	d->deleteAction->setIcon(KIcon( QLatin1String( "edit-delete" )) );
	d->deleteAction->setToolTip(i18n("Delete the selected variable."));
	addRightAction(d->deleteAction, this, SLOT(deleteSelection()));

	addRightStretch();
}


void VariablesWidget::prepareContextualMenu() {

	treeWidget()->addAction(d->newVariableAction);

	treeWidget()->addAction(createSeparator());

	treeWidget()->addAction(d->modifyAction);
	treeWidget()->addAction(d->deleteAction);

	treeWidget()->addAction(createSeparator());

	foreach(QAction* action, crontabWidget()->cutCopyPasteActions()) {
		treeWidget()->addAction(action);
	}

}

void VariablesWidget::toggleModificationActions(bool state) {
	setActionEnabled(d->modifyAction, state);
	setActionEnabled(d->deleteAction, state);
}

void VariablesWidget::toggleNewEntryAction(bool state) {
	setActionEnabled(d->newVariableAction, state);
}

void VariablesWidget::changeCurrentSelection() {
	logDebug() << "Change selection..." << endl;

	bool enabled;
	if (treeWidget()->selectedItems().isEmpty())
		enabled = false;
	else
		enabled = true;

	toggleModificationActions(enabled);
}
