/*
    KT list view item task header.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef VARIABLE_WIDGET_H
#define VARIABLE_WIDGET_H

#include "variablesWidget.h"

class CTVariable;

/**
 * QTreeWidgetItem with a CTTask.
 */
class VariableWidget : public QTreeWidgetItem
{
public:

    /**
     * Initialize the list view item and task.
     */
    explicit VariableWidget(VariablesWidget *variablesWidget, CTVariable *_ctVariable);

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
    CTVariable *getCTVariable() const;

private:

    /**
     * Variable
     */
    CTVariable *ctVariable = nullptr;

    VariablesWidget *variablesWidget = nullptr;
};

#endif // VARIABLE_WIDGET_H
