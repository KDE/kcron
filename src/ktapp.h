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
class KTView;
class CTHost;

/**
 * Application that sets up the main window, reads the config file,
 * sets up the menu bar, toolbar, and status bar.  Obtains the document
 * (here the crontab entries) and give it to the view for display.
 *
 * Provides main window handling, session management and keyboard
 * acceleration.
 */
class KTApp : public KXmlGuiWindow
{
	Q_OBJECT

	friend class KTView;

public:

	// Menu constants
	enum editEntries
	{
		menuEditNew=4,
		menuEditModify,
		menuEditDelete,
		menuEditEnabled=8,
		menuEditRunNow=10
	};

	enum settingsEntries
	{
		menuSettingsShowToolBar,
		menuSettingsShowStatusBar
	};

	static const int statusMessage;

	/**
	 * Initialize the application.
	 */
	KTApp();

	/**
	 * Quit the application..
	 */
	~KTApp();

	/**
	 * Additional init
	 */
	bool init();

	/**
	 * Returns a reference to the document.
	 */
	const CTHost& getCTHost() const;

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

	void statusEditCallback(QAction* action);

	/**
	 * Save document.
	 */
void slotFileSave();

/**
 * Print document.
 */
void slotFilePrint();

/**
 * Close all open windows then quits the application.  If queryClose()
 * returns false because the user canceled the saveModified() dialog, the
 * closing breaks.
 */
void slotFileQuit();

/**
 * Pop up an edit menu.
 */
void slotEdit(const QPoint& qp);

/**
 * Put the marked objects on the clipboard and remove it from the
 * document.
 */
void slotEditCut();

/**
 * Put the marked objects on the clipboard.
 */
void slotEditCopy();

/**
 * Paste the object on clipboard into the document
 */
void slotEditPaste();

/**
 * New.
 */
void slotEditNew();

/**
 * Modify.
 */
void slotEditModify();

/**
 * Delete.
 */
void slotEditDelete();

/**
 * Toggle enabled.
 */
void slotEditEnable();

/**
 * Run program now.
 */
void slotEditRunNow();

/**
 * Changes the status bar contents for the standard label permanently;
 * used to indicate current actions.
 */
void slotStatusMsg(const QString & text);

/**
 * Changes the status message of the whole status bar for two seconds,
 * then restores the last status. This is used to display status bar
 * messages that give information about actions for toolbar icons and
 * menu entries.
 */
void slotStatusHelpMsg(const QString & text);

/** Enables/disables modification buttons
 */
void slotEnableModificationButtons(bool);

/** Enables/disables paste button
 */
void slotEnablePaste(bool);

/** Enables/disables "Run now"
 */
void slotEnableRunNow(bool);

/** Enables/disables "Activated"
 */
void slotEnableEnabled(bool);

private:

/**
 * Disabled copy constructor.
 */
KTApp(const KTApp& source);

/**
 * Disabled assignment operator.
 */
void operator =(const KTApp& source);

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

/**
 * Configuration object of the application.
 */
KSharedConfigPtr config;

/**
 * Main GUI view/working area.
 */
KTView* view;

/**
 * Document object, here crotab enries.
 */
CTHost* cthost;

/**
 * Path to the crontab binary
 */
QString crontab;
};

#endif // KTAPP_H
