/*
    CT Cron Header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CT_GLOBAL_CRON_H
#define CT_GLOBAL_CRON_H

#include <QString>
#include <QList>

#include "ctcron.h"

class CTTask;
class CTVariable;
class CTHost;


/**
 * A user (encapsulation of a single crontab file).  Encapsulates
 * file i/o, parsing, tokenization, and natural language description.
 */
class CTGlobalCron : public CTCron {
public:


	explicit CTGlobalCron(CTHost* ctHost);

	/**
	 * Destructor.
	 */
	virtual ~CTGlobalCron();

	QList<CTTask*> tasks() const Q_DECL_OVERRIDE;
	
	QList<CTVariable*> variables() const Q_DECL_OVERRIDE;
	
	void addTask(CTTask* task) Q_DECL_OVERRIDE;
	void addVariable(CTVariable* variable) Q_DECL_OVERRIDE;
	
	void modifyTask(CTTask* task) Q_DECL_OVERRIDE;
	void modifyVariable(CTVariable* variable) Q_DECL_OVERRIDE;
		
	void removeVariable(CTVariable* variable) Q_DECL_OVERRIDE;
	void removeTask(CTTask* task) Q_DECL_OVERRIDE;

private:
	CTHost* ctHost;
};

#endif // CT_GLOBAL_CRON_H
