/*
    KT main view header.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

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
    explicit CrontabWidget(QWidget* parent, CTHost* ctHost);

	/**
	 * Destructor.
	 */
    ~CrontabWidget() override;
	
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
