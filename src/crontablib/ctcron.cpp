/***************************************************************************
 *   CT Cron Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctcron.h"

#include <QRegExp>
#include <QFile>
#include <QTextStream>
#include <QProcess>

#include <kshell.h>
#include <klocale.h>
#include <ktemporaryfile.h>
#include <klocale.h>

#include "cttask.h"
#include "ctvariable.h"

#include <unistd.h>      // getuid(), unlink()
#include <pwd.h>         // pwd, getpwnam(), getpwuid()

#include "logging.h"

QProcess::ExitStatus CommandLine::execute() {
	QProcess process;
	
	if (standardOutputFile.isEmpty() == false)
		process.setStandardOutputFile(standardOutputFile);
	
	process.start(commandLine, parameters);
	process.waitForFinished(-1);
	
	return process.exitStatus();
}

class CTCronPrivate {
public:

	/**
	 * Indicates whether or not the crontab belongs to the system.
	 */
	bool systemCron;
	
	/**
	 * Indicates if this cron could have tasks and variables from different user
	 */
	bool multiUserCron;
	
	/**
	 * Indicates whether or not the crontab belongs to the current user.
	 */
	bool currentUserCron;

	/**
	 * User  login.
	 */
	QString userLogin;

	/**
	 * User real name.
	 */
	QString userRealName;

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

	int initialTaskCount;
	int initialVariableCount;
	
	CommandLine writeCommandLine;
	
	QString tmpFileName;

	QString error;

	/**
	 * Contains path to the crontab binary file
	 */
	QString crontabBinary;

};

CTCron::CTCron(const QString& crontabBinary, bool systemCron) :
	d(new CTCronPrivate()) {
	
	d->systemCron = systemCron;
	d->multiUserCron = systemCron;
	d->currentUserCron = false;
	
	if (d->multiUserCron == false) {
		logDebug() << "Use this constructor only for system cron" << endl;
		return;
	}

	d->crontabBinary = crontabBinary;

	KTemporaryFile tmp;
	tmp.open();
	d->tmpFileName = tmp.fileName();

	CommandLine readCommandLine;

	// root, so provide requested crontab
	if (systemCron) {
		readCommandLine.commandLine = "cat";
		readCommandLine.parameters << "/etc/crontab";
		readCommandLine.standardOutputFile = d->tmpFileName;

		d->writeCommandLine.commandLine = "cat";
		d->writeCommandLine.parameters << d->tmpFileName;
		d->writeCommandLine.standardOutputFile = "/etc/crontab";

		d->userLogin = i18n("System Crontab");
		d->userRealName = d->userLogin;
	}

	d->initialTaskCount = 0;
	d->initialVariableCount = 0;

	if (isError())
		return;

	
	// Don't set error if it can't be read, it means the user
	// doesn't have a crontab.
	if (readCommandLine.execute() == QProcess::NormalExit) {
		this->parseFile(d->tmpFileName);
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();
}

CTCron::CTCron(const QString& crontabBinary, const struct passwd* userInfos, bool currentUserCron) :
	d(new CTCronPrivate()) {
	
	Q_ASSERT(userInfos != NULL);

	d->multiUserCron = false;
	d->systemCron = false;
	d->currentUserCron = currentUserCron;

	d->crontabBinary = crontabBinary;

	KTemporaryFile tmp;
	tmp.open();
	d->tmpFileName = tmp.fileName();

	CommandLine readCommandLine;
	
	// regular user, so provide user's own crontab
	if (currentUserCron == true) {
		readCommandLine.commandLine = d->crontabBinary;
		readCommandLine.parameters << "-l";
		readCommandLine.standardOutputFile = d->tmpFileName;

		d->writeCommandLine.commandLine = d->crontabBinary;
		d->writeCommandLine.parameters << d->tmpFileName;

	}
	else {

		readCommandLine.commandLine = d->crontabBinary;
		readCommandLine.parameters << "-u" << QString(userInfos->pw_name) << "-l";
		readCommandLine.standardOutputFile = d->tmpFileName;
	
		d->writeCommandLine.commandLine = d->crontabBinary;
		d->writeCommandLine.parameters << "-u" << QString(userInfos->pw_name) << d->tmpFileName;
	}


	d->initialTaskCount = 0;
	d->initialVariableCount = 0;
	
	if (initializeFromUserInfos(userInfos) == false) {
		d->error = i18n("No password entry found for uid '%1'", getuid());
		logDebug() << "Error in crontab creation of" << userInfos->pw_name << endl;
		return;
	}

	// Don't set error if it can't be read, it means the user
	// doesn't have a crontab.
	if (readCommandLine.execute() == QProcess::NormalExit) {
		this->parseFile(d->tmpFileName);
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();
}

CTCron::CTCron(const QString& userLogin) :
	d(new CTCronPrivate()) {
	
	logDebug() << "Initializing a CTGlobalCron from CTCron constructor" << endl;
	
	d->multiUserCron = true;
	d->systemCron = false;
	d->currentUserCron = false;

	d->userLogin = userLogin;
	
}
bool CTCron::initializeFromUserInfos(const struct passwd* userInfos) {
	if (userInfos == 0) {
		return false;
	} else {
		d->userLogin = userInfos->pw_name;
		d->userRealName = userInfos->pw_gecos;
		return true;
	}
}

void CTCron::operator = (const CTCron& source) {
	if (source.isSystemCron() == true) {
		logDebug() << "Trying to affect the system cron" << endl;
	}

	foreach(CTVariable* ctVariable, source.variables()) {
		CTVariable* tmp = new CTVariable(*ctVariable);
		d->variable.append(tmp);
	}

	foreach(CTTask* ctTask, source.tasks()) {
		CTTask* tmp = new CTTask(*ctTask);
		d->task.append(tmp);
	}
}

void CTCron::parseFile(const QString& fileName) {

	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	QString comment("");

	QTextStream in(&file);
	while (in.atEnd() == false) {
		QString line = in.readLine();

		// search for comments "#" but not disabled tasks "#\"
		if ((line.indexOf("#") == 0) && (line.indexOf("\\") != 1)) {
			// If the first 10 characters don't contain a character, it's probably a disabled entry.
			int firstText = line.indexOf(QRegExp("[a-zA-Z]"));
			if (firstText < 0)
				continue;

			if (firstText < 10) {
				// remove leading pound sign
				line = line.mid(1, line.length()-1);
				// remove leading whitespace
				while (line.indexOf(QRegExp("[ \t]")) == 0)
					line = line.mid(1, line.length()-1);
				comment = line;
				continue;
			}
		}

		// either a task or a variable
		int firstWhiteSpace(line.indexOf(QRegExp("[ \t]")));
		int firstEquals(line.indexOf("="));

		// if there is an equals sign and either there is no
		// whitespace or the first whitespace is after the equals
		// sign, it must be a variable
		if ((firstEquals > 0) && ((firstWhiteSpace == -1) || firstWhiteSpace > firstEquals)) {
			// create variable
			CTVariable* tmp = new CTVariable(line, comment, d->userLogin);
			d->variable.append(tmp);
			comment = "";
		}
		// must be a task, either enabled or disabled
		else {
			if (firstWhiteSpace > 0) {
				CTTask* tmp = new CTTask(line, comment, d->userLogin, d->multiUserCron);
				d->task.append(tmp);
				comment = "";
			}
		}
		


	}

}

QString CTCron::exportCron() const {
	QString exportCron;
	exportCron += "# This file was written by KCron.\n";
	exportCron += "# Although KCron supports most crontab formats, use with care when editing.\n";
	exportCron += "# Note: Lines beginning with \"#\\\" indicates a disabled task.\n\n";

	foreach(CTVariable* ctVariable, d->variable) {
		exportCron += ctVariable->exportVariable();
	}

	foreach(CTTask* ctTask, d->task) {
		exportCron += ctTask->exportTask();
	}

	return exportCron;
}

CTCron::~CTCron() {
	foreach(CTTask* ctTask, d->task) {
		delete ctTask;
	}

	foreach(CTVariable* ctVariable, d->variable) {
		delete ctVariable;
	}
	
	delete d;
}

void CTCron::saveToFile(const QString& fileName) {
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	//logDebug() << exportCron() << endl;
	QTextStream out(&file);
	out << exportCron();

	out.flush();
	file.close();
}

void CTCron::save() {
	// write to temp file
	saveToFile(d->tmpFileName);

	// install temp file into crontab
	if (d->writeCommandLine.execute() == QProcess::CrashExit) {
		d->error = i18n("An error occurred while updating crontab.");
	}

	// remove the temp file
	QFile::remove(d->tmpFileName);

	if (isError())
		return;

	// mark as applied
	foreach(CTTask* ctTask, d->task) {
		ctTask->apply();
	}

	foreach(CTVariable* ctVariable, d->variable) {
		ctVariable->apply();
	}

	d->initialTaskCount = d->task.size();
	d->initialVariableCount = d->variable.size();
}

void CTCron::cancel() {
	foreach(CTTask* ctTask, d->task) {
		ctTask->cancel();
	}

	foreach(CTVariable* ctVariable, d->variable) {
		ctVariable->cancel();
	}

}

bool CTCron::isDirty() const {
	if (d->initialTaskCount != d->task.count())
		return true;

	if (d->initialVariableCount != d->variable.count())
		return true;

	foreach(CTTask* ctTask, d->task) {
		if (ctTask->dirty())
		return true;
	}

	foreach(CTVariable* ctVariable, d->variable) {
		if (ctVariable->dirty())
		return true;
	}

	return false;
}

QString CTCron::path() const {
	QString path;

	foreach(CTVariable* ctVariable, d->variable) {
		if (ctVariable->variable == "PATH") {
			path = ctVariable->value;
		}
	}

	return path;

}
/**
 * Return error description
 */
QString CTCron::errorMessage() {
	QString r = d->error;
	d->error = QString();
	return r;
}

bool CTCron::isError() const {
	return ! d->error.isEmpty();
}

QList<CTTask*> CTCron::tasks() const {
	return d->task;
}

QList<CTVariable*> CTCron::variables() const {
	return d->variable;
}

void CTCron::addTask(CTTask* task) {
	if (isSystemCron()) {
		task->setSystemCrontab(true);
	}
	else {
		task->userLogin = d->userLogin;
		task->setSystemCrontab(false);
	}
	
	logDebug() << "Adding task" << task->comment << " user : "<< task->userLogin << endl;

	d->task.append(task);
}

void CTCron::addVariable(CTVariable* variable) {
	if (isSystemCron())
		variable->userLogin = "root";
	else
		variable->userLogin = d->userLogin;


	logDebug() << "Adding variable" << variable->variable << " user : "<< variable->userLogin << endl;
	
	d->variable.append(variable);
}


void CTCron::modifyTask(CTTask* /*task*/) {
	//Nothing to do specifically when modifying a task
}

void CTCron::modifyVariable(CTVariable* /*variable*/) {
	//Nothing to do specifically when modifying a variable
}

void CTCron::removeTask(CTTask* task) {
	d->task.removeAll(task);
}

void CTCron::removeVariable(CTVariable* variable) {
	d->variable.removeAll(variable);
}


bool CTCron::isMultiUserCron() const {
	return d->multiUserCron;
}

bool CTCron::isCurrentUserCron() const {
	return d->currentUserCron;
}

bool CTCron::isSystemCron() const {
	return d->systemCron;
}


QString CTCron::userLogin() const {
	return d->userLogin;
}

QString CTCron::userRealName() const {
	return d->userRealName;
}


