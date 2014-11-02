/***************************************************************************
 *   KT icons implementation.                                              *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "kcronHelper.h"

#include <QFontMetrics>
#include <QTextEdit>
#include <QComboBox>

#include <kglobal.h>

#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"

#include "crontabWidget.h"


void KCronHelper::initUserCombo(QComboBox* userCombo, CrontabWidget* crontabWidget, const QString& selectedUserLogin) {
	int userComboIndex = 0;
	
	QStringList users;
	int selectedIndex = 0;
	foreach(CTCron* ctCron, crontabWidget->ctHost()->crons) {
		if (ctCron->isSystemCron())
			continue;
		
		users.append(ctCron->userLogin());
		
		//Select the actual user
		if (ctCron->userLogin() == selectedUserLogin) {
			selectedIndex = userComboIndex;
		}
	
		userComboIndex++;
	}
	
	users.sort();
	
	userCombo->addItems(users);
	userCombo->setCurrentIndex(selectedIndex);
}

QTextEdit* KCronHelper::createCommentEdit(QWidget* parent) {
	QTextEdit* edit = new QTextEdit(parent);
	edit->setAcceptRichText(false);
	edit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
	edit->setTabChangesFocus(true);

	QFontMetrics fontMetrics(edit->currentFont());
	edit->setMaximumHeight(fontMetrics.lineSpacing() * 3); //TODO Choose a smarter value
	
	return edit;
}
