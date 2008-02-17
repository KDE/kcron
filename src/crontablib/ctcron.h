/***************************************************************************
 *   CT Cron Header                                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef CTCRON_H
#define CTCRON_H

#include <QString>
#include <QList>
#include <QStringList>
#include <QProcess>

class CTTask;
class CTVariable;

struct passwd;

class CommandLine {
public:
	QString commandLine;
	
	QStringList parameters;
	
	QString standardOutputFile;
	
	QProcess::ExitStatus execute();
};

/**
 * A user (encapsulation of a single crontab file).  Encapsulates
 * file i/o, parsing, tokenization, and natural language description.
 */
class CTCron {
public:

	/**
	 * Constructs the scheduled tasks, environment variables from crontab
	 * files and obtains some information about the user from the system.
	 *
	 * Default is to construct from the user's crontab.  Can also be called,
	 * passing TRUE, to construct from the system crontab.  Throws an
	 * exception if the crontab file can not be found, read, or parsed.
	 */
	explicit CTCron(const QString& cronBinary, bool _syscron = false, const QString& _login = "");

	/**
	 * If you already have a struct passwd, use it instead. This
	 * is never used for the system crontab.
	 */
	explicit CTCron(const QString& cronBinary, const struct passwd * _login = 0L);

	/**
	 * Copy one user's tasks and environement variables to another user.
	 */
	void operator =(const CTCron& source);
	
	

	/**
	 * Tokenizes to crontab file format.
	 */
	QString exportCron();

	/**
	 * Apply changes.
	 */
	void apply();

	/**
	 * Cancel changes.
	 */
	void cancel();

	/**
	 * Indicates whether or not dirty.
	 */
	bool dirty();

	/**
	 * Returns the PATH environment variable value.  A short cut to iterating
	 * the tasks vector.
	 */
	QString path() const;

	/**
	 * Returns whether an error has occurred
	 */
	bool isError() {
		return !error.isEmpty();
	}

	/**
	 * Return error description
	 */
	QString errorMessage() {
		QString r = error;
		error = QString();
		return r;
	}

	/**
	 * Indicates whether or not the crontab belongs to the system.
	 */
	const bool syscron;

	/**
	 * User  login.
	 */
	QString login;

	/**
	 * User real name.
	 */
	QString name;

	/**
	 * User's scheduled tasks.
	 */
	QList<CTTask *> task;

	/**
	 * User's environment variables.  Note:  These are only environment variables
	 * found in the user's crontab file and does not include any set in a 
	 * login or shell script such as ".bash_profile".
	 */
	QList<CTVariable *> variable;

	/**
	 * Destructor.
	 */
	~CTCron();

private:

	/**
	 * Parses crontab file format.
	 */
	void parseFile(const QString& fileName);

	void saveToFile(const QString& fileName);
	
	/**
	 * Can't copy a user!
	 */
	CTCron(const CTCron& source);

	int initialTaskCount;
	int initialVariableCount;
	
	CommandLine writeCommandLine;
	
	QString tmpFileName;

	QString error;

	/**
	 * Contains path to the crontab binary file
	 */
	QString crontab;

protected:
	// Initialize member variables from the struct passwd.
	bool initFromPasswd(const struct passwd *);
};

#endif // CTCRON_H
