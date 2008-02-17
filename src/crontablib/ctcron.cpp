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
#include <stdio.h>
#include <assert.h>

QProcess::ExitStatus CommandLine::execute() {
	QProcess process;
	
	if (standardOutputFile.isEmpty() == false)
		process.setStandardOutputFile(standardOutputFile);
	
	process.start(commandLine, parameters);
	process.waitForFinished(-1);
	
	return process.exitStatus();
}

CTCron::CTCron(const QString& crontabBinary, bool _syscron, const QString& _login) :
	syscron(_syscron) {
	int uid(getuid());

	this->crontab = crontabBinary;

	KTemporaryFile tmp;
	tmp.open();
	tmpFileName = tmp.fileName();

	CommandLine readCommandLine;

	// root, so provide requested crontab
	if (uid == 0) {
		if (syscron) {
			readCommandLine.commandLine = "cat";
			readCommandLine.parameters << "/etc/crontab";
			readCommandLine.standardOutputFile = tmpFileName;

			writeCommandLine.commandLine = "cat";
			writeCommandLine.parameters << tmpFileName;
			writeCommandLine.standardOutputFile = "/etc/crontab";

			login = i18n("(System Crontab)");
			name = "";
		} else {
			readCommandLine.commandLine = crontab;
			readCommandLine.parameters << "-u" << _login << "-l";
			readCommandLine.standardOutputFile = tmpFileName;
			
			writeCommandLine.commandLine = crontab;
			writeCommandLine.parameters << "-u" << _login << tmpFileName;

			if (!initFromPasswd(getpwnam(_login.toLocal8Bit()))) {
				error = i18n("No password entry found for user '%1'", _login);
			}
		}
	}
	// regular user, so provide user's own crontab
	else {
		readCommandLine.commandLine = crontab;
		readCommandLine.parameters << "-l";
		readCommandLine.standardOutputFile = tmpFileName;

		writeCommandLine.commandLine = crontab;
		writeCommandLine.parameters << tmpFileName;

		if (!initFromPasswd(getpwuid(uid))) {
			error = i18n("No password entry found for uid '%1'", uid);
		}
	}

	if (name.isEmpty())
		name = login;

	initialTaskCount = 0;
	initialVariableCount = 0;

	if (isError())
		return;

	
	// Don't set error if it can't be read, it means the user
	// doesn't have a crontab.
	if (readCommandLine.execute() == QProcess::NormalExit) {
		this->parseFile(tmpFileName);
	}

	initialTaskCount = task.size();
	initialVariableCount = variable.size();
}

CTCron::CTCron(const QString& crontabBinary, const struct passwd *pwd) :
	syscron(false) {
	Q_ASSERT(pwd != 0L);

	crontab = crontabBinary;

	KTemporaryFile tmp;
	tmp.open();
	tmpFileName = tmp.fileName();

	CommandLine readCommandLine;
	readCommandLine.commandLine = crontab;
	readCommandLine.parameters << "-u" << QString(pwd->pw_name) << "-l";
	readCommandLine.standardOutputFile = tmpFileName;

	writeCommandLine.commandLine = crontab;
	writeCommandLine.parameters << "-u" << QString(pwd->pw_name) << tmpFileName;

	initFromPasswd(pwd);

	initialTaskCount = 0;
	initialVariableCount = 0;

	if (isError())
		return;

	// Don't set error if it can't be read, it means the user
	// doesn't have a crontab.
	if (readCommandLine.execute() == QProcess::NormalExit) {
		this->parseFile(tmpFileName);
	}

	initialTaskCount = task.size();
	initialVariableCount = variable.size();
}

bool CTCron::initFromPasswd(const struct passwd *pwd) {
	if (pwd == 0) {
		return false;
	} else {
		login = pwd->pw_name;
		name = pwd->pw_gecos;
		return true;
	}
}

void CTCron::operator = (const CTCron& source) {
	assert(!source.syscron);

	foreach(CTVariable* ctVariable, source.variable) {
		CTVariable* tmp = new CTVariable(*ctVariable);
		variable.append(tmp);
	}

	foreach(CTTask* ctTask, source.task) {
		CTTask* tmp = new CTTask(*ctTask);
		task.append(tmp);
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
			CTVariable* tmp = new CTVariable(line, comment);
			variable.append(tmp);
			comment = "";
		}
		// must be a task, either enabled or disabled
		else {
			if (firstWhiteSpace > 0) {
				CTTask* tmp = new CTTask(line, comment, syscron);
				task.append(tmp);
				comment = "";
			}
		}
		


	}

}

QString CTCron::exportCron() {
	QString exportCron;
	exportCron += "# This file was written by KCron.\n";
	exportCron += "# Although KCron supports most crontab formats, use care when editing.\n";
	exportCron += "# Note: Lines beginning with \"#\\\" indicates a disabled task.\n\n";

	foreach(CTVariable* ctVariable, variable) {
		exportCron += ctVariable->exportVariable();
	}

	foreach(CTTask* ctTask, task) {
		exportCron += ctTask->exportTask();
	}

	return exportCron;
}

CTCron::~CTCron() {
	foreach(CTTask* ctTask, task) {
		delete ctTask;
	}

	foreach(CTVariable* ctVariable, variable) {
		delete ctVariable;
	}
}

void CTCron::saveToFile(const QString& fileName) {
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
		return;

	QTextStream out(&file);
	out << exportCron();

	out.flush();
	file.close();
}

void CTCron::apply() {
	// write to temp file
	saveToFile(tmpFileName);

	// install temp file into crontab
	if (writeCommandLine.execute() == QProcess::CrashExit) {
		error = i18n("An error occurred while updating crontab.");
	}

	// remove the temp file
	QFile::remove(tmpFileName);

	if (isError())
		return;

	// mark as applied
	foreach(CTTask* ctTask, task) {
		ctTask->apply();
	}

	foreach(CTVariable* ctVariable, variable) {
		ctVariable->apply();
	}

	initialTaskCount = task.size();
	initialVariableCount = variable.size();
}

void CTCron::cancel() {
	foreach(CTTask* ctTask, task) {
		ctTask->cancel();
	}

	foreach(CTVariable* ctVariable, variable) {
		ctVariable->cancel();
	}

}

bool CTCron::dirty() {
	if (initialTaskCount != task.count())
		return true;

	if (initialVariableCount != variable.count())
		return true;

	foreach(CTTask* ctTask, task) {
		if (ctTask->dirty())
		return true;
	}

	foreach(CTVariable* ctVariable, variable) {
		if (ctVariable->dirty())
		return true;
	}

	return false;
}

QString CTCron::path() const {
	QString path;

	foreach(CTVariable* ctVariable, variable) {
		if (ctVariable->variable == "PATH") {
			path = ctVariable->value;
		}
	}

	return path;

}
