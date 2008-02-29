/***************************************************************************
 *   KT icons.                                                             *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KCM_CRON_H
#define KCM_CRON_H

#include <QPixmap>
#include <QString>
#include <QWidget>
#include <QVariantList>
#include <QStringList>

#include <kcmodule.h>
#include <ksharedconfig.h>

class KActionCollection;

class CrontabWidget;
class CTHost;

class KCMCronPrivate;

class KCMCron : public KCModule {
	Q_OBJECT

public:
	//, const QVariantList& arguments
	KCMCron(QWidget* parent, const QStringList& args);

	~KCMCron();

	virtual void load();
	virtual void save();
	virtual void defaults();
	
	/**
	 * Additional init
	 */
	bool init();

	/**
	 * Returns a reference to the CTHost.
	 */
	CTHost* ctHost() const;
	
public slots:


	/**
	 * Changes the status bar contents for the standard label permanently;
	 * used to indicate current actions.
	 */
	void slotStatusMessage(const QString & text);

	/**
	 * Changes the status message of the whole status bar for two seconds,
	 * then restores the last status. This is used to display status bar
	 * messages that give information about actions for toolbar icons and
	 * menu entries.
	 */
	void slotStatusHelpMessage(const QString & text);
	
	void displayActionInformation(QAction* action);

	/** Enables/disables paste button
	 */
	void togglePasteAction(bool enabled);

	/** Enables/disables modification buttons
	 */
	void toggleModificationActions(bool enabled);

	/** Enables/disables "Run now"
	 */
	void toggleRunNowActions(bool enabled);
	
	void toggleNewEntryActions(bool enabled);

private:
	
	/**
	 * TODO Replace this if possible
	 */
	KActionCollection* actionCollection();

	/**
	 * Initialize actions.
	 */
	void setupActions();
	
	void prepareTasksWidgetContextualMenu();
	void prepareVariablesWidgetContextualMenu();
	
	QAction* createSeparator();

	KCMCronPrivate* const d;
};

#endif // KCM_CRON_H
