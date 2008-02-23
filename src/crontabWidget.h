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
#include <QResizeEvent>

#include "tasksWidget.h"
#include "variablesWidget.h"

class QString;
class CTHost;
class KTPrint;
class CTCron;
class CTVariable;
class CTTask;

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
	
	TasksWidget* tasksWidget();
	
	VariablesWidget* variablesWidget();

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

signals:

	/** Enables/disables modification 
	 * buttons depending
	 * on if a task is selected
	 */
	void modificationActionsToggled(bool);

	/** 
	 * Enables/disables paste button depending
	 * on clipboard contents
	 */
	void pasteActionToggled(bool);

	/** Enables/disables "Run now" depending
	 * on the task settings
	 */
	void runNowActionToggled(bool);

protected slots:

	void slotSetCurrentItem();

private:
	
	/**
	 * Print page heading.
	 */
	void pageHeading(/*KTListItem* user, */KTPrint& printer) const;

	/**
	 * Print page footer.
	 */
	void pageFooter(/*KTListItem* user, */KTPrint& printer) const;

	/**
	 * Initialize view from underlying objects.
	 */
	void initialize();

	CrontabWidgetPrivate* const d;
	
};

#endif // KTVIEW_H
