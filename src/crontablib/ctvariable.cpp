/***************************************************************************
 *   CT Environment Variable Implementation                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "ctvariable.h"

#include <QRegExp>

#include <klocale.h>
#include <kiconloader.h>

#include "kcronIcons.h"

#include "ctHelper.h"

CTVariable::CTVariable(const QString& tokenString, const QString& _comment, const QString& _userLogin) {

	QString tokStr = tokenString;

	if (tokStr.mid(0, 2) == "#\\") {
		tokStr = tokStr.mid(2, tokStr.length() - 2);
		enabled = false;
	} else
		enabled = true;

	int spacepos(0);

	spacepos = tokStr.indexOf(QRegExp("[ =]"));
	variable = tokStr.mid(0, spacepos);

	value = tokStr.mid(spacepos+1, tokStr.length()-spacepos-1);
	comment = _comment;

	userLogin = _userLogin;

	initialVariable = variable;
	initialValue = value;
	initialComment = comment;
	initialUserLogin = userLogin;
	initialEnabled = enabled;

}

CTVariable::CTVariable(const CTVariable &source) :
	variable(source.variable), value(source.value), comment(source.comment), userLogin(source.userLogin), enabled(source.enabled), initialVariable(""), initialValue(""), initialComment(""), initialUserLogin(""), initialEnabled(true) {
}

CTVariable& CTVariable::operator = (const CTVariable& source) {
	if (this == &source)
		return *this;

	variable = source.variable;
	value = source.value;
	comment = source.comment;
	userLogin = source.userLogin;
	enabled = source.enabled;

	initialVariable = "";
	initialValue = "";
	initialComment = "";
	initialUserLogin = "";
	initialEnabled = true;
	return *this;
}

QString CTVariable::exportVariable() {
	QString exportVariable;

	exportVariable += CTHelper::exportComment(comment);

	if (enabled == false)
		exportVariable += "#\\";

	exportVariable += variable + "=" + value + "\n";

	return exportVariable;
}

void CTVariable::apply() {
	initialVariable = variable;
	initialValue = value;
	initialComment = comment;
	initialUserLogin = userLogin;

	initialEnabled = enabled;
}

void CTVariable::cancel() {
	variable = initialVariable;
	value = initialValue;
	comment = initialComment;
	userLogin = initialUserLogin;
	enabled = initialEnabled;
}

bool CTVariable::dirty() const {
	return ((variable != initialVariable) || (value != initialValue) || (comment != initialComment) || (userLogin != initialUserLogin) || (enabled != initialEnabled));
}

QPixmap CTVariable::variableIcon() const {
	if (variable == "MAILTO")
		return SmallIcon("mail-message");
	else if (variable == "SHELL")
		return SmallIcon("utilities-terminal");
	else if (variable == "HOME")
		return SmallIcon("go-home");
	else if (variable == "PATH")
		return SmallIcon("folder");
	else if (variable == "LD_CONFIG_PATH")
		return SmallIcon("application-x-sharedlib");

	return KCronIcons::variable(KCronIcons::Small);

}

QString CTVariable::information() const {

	if (variable == "HOME") {
		return i18n("Override default home folder.");
	}
	else if (variable == "MAILTO") {
		return i18n("Email output to specified account.");
	}
	else if (variable == "SHELL") {
		return i18n("Override default shell.");
	}
	else if (variable == "PATH") {
		return i18n("Folders to search for program files.");
	}
	else if (variable == "LD_CONFIG_PATH") {
		return i18n("Dynamic libraries location.");
	}
	
	return i18n("Local Variable");
	
}
