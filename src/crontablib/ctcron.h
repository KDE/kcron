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

class CTCronPrivate;

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
	explicit CTCron(const QString& cronBinary, bool systemCron);

	/**
	 * If you already have a struct passwd, use it instead. This
	 * is never used for the system crontab.
	 */
	explicit CTCron(const QString& cronBinary, const struct passwd* userInfos, bool currentUserCron);

	/**
	 * Destructor.
	 */
	~CTCron();

	/**
	 * Copy one user's tasks and environement variables to another user.
	 */
	void operator =(const CTCron& source);
	
	

	/**
	 * Tokenizes to crontab file format.
	 */
	QString exportCron() const;

	/**
	 * Apply changes.
	 */
	void save();

	/**
	 * Cancel changes.
	 */
	void cancel();

	/**
	 * Indicates whether or not dirty.
	 */
	bool isDirty() const;

	/**
	 * Returns the PATH environment variable value.  A short cut to iterating
	 * the tasks vector.
	 */
	QString path() const;

	/**
	 * Returns whether an error has occurred
	 */
	bool isError() const;

	/**
	 * Return error description
	 */
	QString errorMessage();
	
	QList<CTTask*> tasks() const;
	
	QList<CTVariable*> variables() const;
	
	bool isSystemCron() const;
	
	bool isCurrentUserCron() const;
	
	QString userLogin() const;
	
	QString userRealName() const;

protected:
	// Initialize member variables from the struct passwd.
	bool initializeFromUserInfos(const struct passwd* userInfos);
	
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

	CTCronPrivate* const d;
};

#endif // CTCRON_H
