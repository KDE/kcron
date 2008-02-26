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

#ifndef KTVIEW_H
#define KTVIEW_H

#include <QWidget>

#include "tasksWidget.h"
#include "variablesWidget.h"

class QString;
class QHBoxLayout;
class QAbstractButton;


class CTHost;
class CTCron;
class CTVariable;
class CTTask;

class CrontabPrinter;
class TaskWidget;
class VariableWidget;

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

public slots:
	
	/**
	 * Print crontab.
	 */
	void print();
	
	/**
	 * Copies variables and/or tasks.
	 */
	void copy();
	
	void cut();
	
	/**
	 * Pastes variables and/or tasks from the clipboard.
	 */
	void paste();
	
	CTCron* currentCron() const;

signals:

	/** Enables/disables modification 
	 * buttons depending
	 * on if a task is selected
	 */
	void modificationActionsToggled(bool enabled);

	/** 
	 * Enables/disables "Run now" depending
	 * on the task settings
	 */
	void runNowActionToggled(bool enabled);
	
	/**
	 * Toggle new entries
	 */
	void newEntryToggled(bool enabled);

	/** 
	 * Enables/disables paste button depending
	 * on clipboard contents
	 */
	void pasteActionToggled(bool enabled);

protected slots:

	void slotSetCurrentItem();

	void refreshCron();

	void checkOtherUsers();
	
private:

	/**
	 * Initialize view from underlying objects.
	 */
	void initialize();
	
	QHBoxLayout* createCronSelector();
	
	bool hasClipboardContent();
	
	CrontabWidgetPrivate* const d;
	
};

#endif // KTVIEW_H
