/***************************************************************************
 *   --------------------------------------------------------------------  *
 *   CT Host Implementation                                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "cthost.h"

#include "ctcron.h"
#include <klocale.h>

#include <unistd.h>  // getuid()
#include <sys/types.h>
#include <pwd.h>

#include <QFile>

#include <QTextStream>

CTHost::CTHost(const QString& cronBinary) {
	struct passwd *pwd = 0L;

	this->crontabBinary = cronBinary;

	// If it is the root user
	if (getuid() == 0) {
		// Create the system cron table.
		createCTCron(true);

		// Read /etc/passwd
		setpwent(); // restart
		while ((pwd=getpwent())) {
			if (allowDeny(pwd->pw_name))
				createCTCron(pwd);
		}
		setpwent(); // restart again for others
	} else
	// Non-root user, so just create user's cron table.
	{
		// Get name from UID, check it against AllowDeny()
		unsigned int uid = getuid();
		setpwent(); // restart
		while ((pwd=getpwent())) {
			if ((pwd->pw_uid == uid) && (!allowDeny(pwd->pw_name))) {
				error = i18n("You have been blocked from using KCron\
                      by either the /etc/cron.allow file or the /etc/cron.deny file.\
                      \n\nCheck the crontab man page for further details.");
				return;
			}
		}
		setpwent(); // restart again for others
		createCTCron();
	}
}

CTHost::~CTHost() {
	foreach(CTCron* ctCron, cron) {
		delete ctCron;
	}
}

bool CTHost::allowDeny(char *name) {
	QFile allow("/etc/cron.allow");

	// if cron.allow exists make sure user is listed
	if (allow.open(IO_ReadOnly)) {
		QTextStream stream(&allow);
		while (!stream.atEnd()) {
			if (stream.readLine() == name) {
				allow.close();
				return true;
			}
		}
		allow.close();
		return false;
	} else {
		allow.close();
		QFile deny("/etc/cron.deny");

		// else if cron.deny exists make sure user is not listed
		if (deny.open(IO_ReadOnly)) {
			QTextStream stream(&deny);
			while (!stream.atEnd()) {
				if (stream.readLine() == name) {
					deny.close();
					return false;
				}
			}
			deny.close();
			return true;
		} else {
			deny.close();
			return true;
		}
	}
}

void CTHost::apply() {
	foreach(CTCron* ctCron, cron) {
		ctCron->apply();
		if (ctCron->isError()) {
			error = ctCron->errorMessage();
			return;
		}
	}
}

void CTHost::cancel() {
	foreach(CTCron* ctCron, cron) {
		ctCron->cancel();
	}
}

bool CTHost::dirty() {
	bool isDirty(false);

	foreach(CTCron* ctCron, cron) {
		if (ctCron->dirty()) {
			isDirty = true;
		}
	}

	return isDirty;
}

CTCron* CTHost::createCTCron(bool _syscron, const QString& _login) {
	CTCron* p = new CTCron(crontabBinary, _syscron, _login);
	if (p->isError()) {
		error = p->errorMessage();
		delete p;
		return 0;
	}

	cron.append(p);

	return p;
}

CTCron* CTHost::createCTCron(const struct passwd *pwd) {
	CTCron* p = new CTCron(crontabBinary, pwd);
	if (p->isError()) {
		error = p->errorMessage();
		delete p;
		return 0;
	}
	cron.append(p);
	return p;
}

bool CTHost::root() const {
	return (!getuid());
}
