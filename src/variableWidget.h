/***************************************************************************
 *   KT list view item task header.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef VARIABLE_WIDGET_H
#define VARIABLE_WIDGET_H

#include <QIcon>

#include "variablesWidget.h"


class CTVariable;

/**
 * QTreeWidgetItem with a CTTask.
 */
class VariableWidget : public QTreeWidgetItem {
public:

	/**
	 * Initialize the list view item and task.
	 */
	VariableWidget(VariablesWidget* variablesWidget, CTVariable* _ctVariable);
	
	/**
	 * Refresh from underlying task.
	 */
	void refresh();
	
	/*
	 * Change the status of this variable
	 */
	void toggleEnable();

	/**
	 * Get the task.
	 */
	CTVariable* getCTVariable() const;

private:

	QIcon findIcon() const;
	
	/**
	 * Variable
	 */
	CTVariable* ctVariable;
	
	VariablesWidget* variablesWidget;
};

#endif // VARIABLE_WIDGET_H
