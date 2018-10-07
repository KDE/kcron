/***************************************************************************
 *   KT list view item cron tasks folder.                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

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

signals:
	void variableModified(bool);
	
public slots:
	void modifySelection();
	
	void deleteSelection() Q_DECL_OVERRIDE;

	/**
	 * Create a new variable.  Default is which type is most recently selected.
	 */
	void createVariable();
	
	void addVariable(CTVariable* variable);

	void changeCurrentSelection();

protected slots:
	void modifySelection(QTreeWidgetItem* item, int position) Q_DECL_OVERRIDE;

private:
	void refreshHeaders();
	
	int statusColumnIndex();
	
	void setupActions();
	
	void prepareContextualMenu();

	VariablesWidgetPrivate* const d;
};

#endif // VARIABLES_WIDGET_H
