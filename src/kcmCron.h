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

class KCronPrivate;

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
	CTHost* getCTHost() const;

protected:

	/** Called on window close event.  Asks the document if it is dirty
	 * and if so, prompts the user for saving before exiting..
	 */
	virtual bool queryClose();

	/** Called when the last window of the application is going to be
	 * closed.  Saves options.
	 */
	virtual bool queryExit();

public slots:

	/**
	 * Save document.
	 */
	void slotSave();

	/**
	 * Close all open windows then quits the application.  If queryClose()
	 * returns false because the user canceled the saveModified() dialog, the
	 * closing breaks.
	 */
	void slotQuit();

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
	
	KActionCollection* actionCollection();

	/**
	 * Get application caption.
	 */
	QString caption();

	/**
	 * Initialize actions.
	 */
	void setupActions();

	/**
	 * Initialize status bar.
	 */
	void initStatusBar();

	/**
	 * Read general options again and initialize all variables like the
	 * geometry.
	 */
	void readOptions();

	/**
	 * Save general options like all bar positions and status as well as the
	 * geometry to the configuration file.
	 */
	void saveOptions();
	
	void prepareTasksWidgetContextualMenu();
	void prepareVariablesWidgetContextualMenu();
	
	QAction* createSeparator();


	KCronPrivate* const d;
};

#endif // KCM_CRON_H
