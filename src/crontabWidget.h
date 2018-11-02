/***************************************************************************
 *   KT main view header.                                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CRONTABWIDGET_H
#define CRONTABWIDGET_H

#include <QWidget>

#include "tasksWidget.h"
#include "variablesWidget.h"

class QHBoxLayout;


class CTHost;
class CTCron;


class CrontabWidgetPrivate;

/**
 * Main GUI view of the crontab entries.
 */
class CrontabWidget : public QWidget {
Q_OBJECT

public:

	/**
	 * Initializes view.
	 */
	CrontabWidget(QWidget* parent, CTHost* ctHost);

	/**
	 * Destructor.
	 */
	~CrontabWidget();
	
	TasksWidget* tasksWidget() const;
	
	VariablesWidget* variablesWidget() const;
	
	CTHost* ctHost() const;
	
	CTCron* currentCron() const;

	QList<QAction*> cutCopyPasteActions();
	QAction* printAction();
	
public Q_SLOTS:
	
	/**
	 * Copies variables and/or tasks.
	 */
	void copy();
	
	void cut();
	
	/**
	 * Pastes variables and/or tasks from the clipboard.
	 */
	void paste();
	
	/**
	 * Print crontab.
	 */
	void print();

protected Q_SLOTS:

	void refreshCron();

	void checkOtherUsers();
	
private:

	/** 
	 * Enables/disables paste button
	 */
	void togglePasteAction(bool enabled);

	/** 
	 * Enables/disables modification buttons
	 */
	void toggleModificationActions(bool enabled);
	
	/** 
	 * Enables/disables new entry actions
	 */
	void toggleNewEntryActions(bool enabled);
	
	/**
	 * Initialize actions.
	 */
	void setupActions();

	/**
	 * Initialize view from underlying objects.
	 */
	void initialize();
	
	QHBoxLayout* createCronSelector();
	
	bool hasClipboardContent();
	
	CrontabWidgetPrivate* const d;
	
};

#endif // CRONTABWIDGET_H
