/*
    KT list view item cron tasks folder.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef VARIABLES_WIDGET_H
#define VARIABLES_WIDGET_H

#include <QTreeWidget>

#include "genericListWidget.h"
#include "cthost.h"

class VariableWidget;
class CTVariable;

class VariablesWidgetPrivate;

/**
 * QListViewItem of a "tasks" folder.
 */
class VariablesWidget : public GenericListWidget {
	Q_OBJECT

public:

	/**
	 * Construct tasks folder from branch.
	 */
	explicit VariablesWidget(CrontabWidget* crontabWidget);
	
	virtual ~VariablesWidget();

	QList<VariableWidget*> selectedVariablesWidget() const;
	
	VariableWidget* firstSelectedVariableWidget() const;
	
	void refreshVariables(CTCron* cron);
	
	bool needUserColumn();

	/** 
	 * Enables/disables modification buttons
	 */
	void toggleModificationActions(bool enabled);
	
	/** 
	 * Enables/disables new entry actions
	 */
	void toggleNewEntryAction(bool enabled);

Q_SIGNALS:
	void variableModified(bool);
	
public Q_SLOTS:
	void modifySelection();
	
	void deleteSelection() override;

	/**
	 * Create a new variable.  Default is which type is most recently selected.
	 */
	void createVariable();
	
	void addVariable(CTVariable* variable);

	void changeCurrentSelection();

protected Q_SLOTS:
	void modifySelection(QTreeWidgetItem* item, int position) override;

private:
	void refreshHeaders();
	
	int statusColumnIndex();
	
	void setupActions();
	
	void prepareContextualMenu();

	VariablesWidgetPrivate* const d;
};

#endif // VARIABLES_WIDGET_H
