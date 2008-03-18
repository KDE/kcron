/***************************************************************************
 *   KT icons.                                                             *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KCRON_HELPER_H
#define KCRON_HELPER_H

#include <QPixmap>
#include <QString>

class QComboBox;
class QTextEdit;

class CrontabWidget;


class KCronHelper {

public:
	static void initUserCombo(QComboBox* userCombo, CrontabWidget* crontabWidget, const QString& selectedUserLogin);
	
	static QTextEdit* createCommentEdit(QWidget* parent); 
};

#endif // KCRON_HELPER_H
