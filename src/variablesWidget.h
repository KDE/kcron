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

#include "ktprint.h"

#include "genericListWidget.h"
#include "cthost.h"

class VariableWidget;

/**
 * QListViewItem of a "tasks" folder.
 */
class VariablesWidget : public GenericListWidget {
	Q_OBJECT

public:

	/**
	 * Construct tasks folder from branch.
	 */
	VariablesWidget(QWidget* parent, CTHost* ctHost);

	/**
	 * Print all tasks.
	 */
	void print(KTPrint& printer);

	VariableWidget* firstSelectedVariableWidget() const;

public slots:
	void modifySelection();
	
	void deleteSelection();

	/**
	 * Create a new variable.  Default is which type is most recently selected.
	 */
	void createVariable();

protected slots:
	void modifySelection(QTreeWidgetItem* item, int position);
	
private:
	int statusColumnIndex();

};

#endif // VARIABLES_WIDGET_H
