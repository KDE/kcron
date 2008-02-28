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

void CTVariable::operator = (const CTVariable& source) {
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
	return;
}

QString CTVariable::exportVariable() {
	QString exportVariable;

	if (comment.isEmpty() == false)
		exportVariable += "# " + comment + "\n";

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
