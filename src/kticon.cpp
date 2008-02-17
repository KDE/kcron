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

#include "kticon.h"

#include <QImage>
#include <QPixmap>

#include <kglobal.h>
#include <kiconloader.h>

QPixmap KTIcon::getIcon(const QString&name, KTIcon::IconSize size) {
	if (size == KTIcon::Small)
		return SmallIcon(name);
	else if (size == KTIcon::Normal)
		return BarIcon(name);
	
	return DesktopIcon(name);
}

QPixmap KTIcon::application(KTIcon::IconSize size) {
	return getIcon("kcron", size);
}

QPixmap KTIcon::system(KTIcon::IconSize size) {
	return getIcon("utilities-terminal", size);
}

QPixmap KTIcon::user(KTIcon::IconSize size) {
	return getIcon("user-identity", size);
}

QPixmap KTIcon::variables(KTIcon::IconSize size) {
	return getIcon("folder", size);
}

QPixmap KTIcon::variable(KTIcon::IconSize size) {
	return getIcon("text", size);
}

QPixmap KTIcon::mail(KTIcon::IconSize size) {
	return getIcon("mail-message", size);
}

QPixmap KTIcon::shell(KTIcon::IconSize size) {
	return getIcon("utilities-terminal", size);
}

QPixmap KTIcon::home(KTIcon::IconSize size) {
	return getIcon("go-home", size);
}

QPixmap KTIcon::path(KTIcon::IconSize size) {
	return getIcon("folder", size);
}

QPixmap KTIcon::tasks(KTIcon::IconSize size) {
	return getIcon("folder", size);
}

QPixmap KTIcon::task(KTIcon::IconSize size) {
	return getIcon("system-run", size);
}

QPixmap KTIcon::information(KTIcon::IconSize size) {
	return getIcon("dialog-information", size);
}

QPixmap KTIcon::error(KTIcon::IconSize size) {
	return getIcon("dialog-error", size);
}
