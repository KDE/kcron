/***************************************************************************
 *   KT application header.                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTAPP_H
#define KTAPP_H

#include <kxmlguiwindow.h>
#include <ksharedconfig.h>

class QString;
class CrontabWidget;
class CTHost;

class KCronPrivate;

/**
 * Application that sets up the main window, reads the config file,
 * sets up the menu bar, toolbar, and status bar.  Obtains the document
 * (here the crontab entries) and give it to the view for display.
 *
 * Provides main window handling, session management and keyboard
 * acceleration.
 */
class KCron : public KXmlGuiWindow {
	Q_OBJECT

public:

	/**
	 * Initialize the application.
	 */
	KCron();

	/**
	 * Quit the application..
	 */
	~KCron();

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
	 * Print document.
	 */
	void slotPrint();

	/**
	 * Close all open windows then quits the application.  If queryClose()
	 * returns false because the user canceled the saveModified() dialog, the
	 * closing breaks.
	 */
	void slotQuit();

	/**
	 * Put the marked objects on the clipboard.
	 */
	void slotCopy();

	/**
	 * Paste the object on clipboard into the document
	 */
	void slotPaste();

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
	 * Disabled copy constructor.
	 */
	KCron(const KCron& source);

	/**
	 * Disabled assignment operator.
	 */
	void operator =(const KCron& source);

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

#endif // KTAPP_H
