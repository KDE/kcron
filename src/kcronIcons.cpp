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

#include "kcronIcons.h"

#include <QImage>
#include <QPixmap>

#include <kglobal.h>
#include <kiconloader.h>

QPixmap KCronIcons::getIcon(const QString&name, KCronIcons::IconSize size) {
	if (size == KCronIcons::Small)
		return SmallIcon(name);
	else if (size == KCronIcons::Normal)
		return BarIcon(name);
	
	return DesktopIcon(name);
}

QPixmap KCronIcons::application(KCronIcons::IconSize size) {
	return getIcon("kcron", size);
}

QPixmap KCronIcons::system(KCronIcons::IconSize size) {
	return getIcon("utilities-terminal", size);
}

QPixmap KCronIcons::user(KCronIcons::IconSize size) {
	return getIcon("user-identity", size);
}

QPixmap KCronIcons::variable(KCronIcons::IconSize size) {
	return getIcon("text", size);
}

QPixmap KCronIcons::mail(KCronIcons::IconSize size) {
	return getIcon("mail-message", size);
}

QPixmap KCronIcons::shell(KCronIcons::IconSize size) {
	return getIcon("utilities-terminal", size);
}

QPixmap KCronIcons::home(KCronIcons::IconSize size) {
	return getIcon("go-home", size);
}

QPixmap KCronIcons::path(KCronIcons::IconSize size) {
	return getIcon("folder", size);
}

QPixmap KCronIcons::task(KCronIcons::IconSize size) {
	return getIcon("system-run", size);
}

QPixmap KCronIcons::information(KCronIcons::IconSize size) {
	return getIcon("dialog-information", size);
}

QPixmap KCronIcons::error(KCronIcons::IconSize size) {
	return getIcon("dialog-error", size);
}
