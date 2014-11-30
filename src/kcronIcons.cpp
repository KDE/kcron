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

#include <QPixmap>

#include <kiconloader.h>

QPixmap KCronIcons::getIcon(const QString&name, KCronIcons::IconSize size) {
	if (size == KCronIcons::Small)
		return SmallIcon(name);
	else if (size == KCronIcons::Normal)
		return BarIcon(name);

	return DesktopIcon(name);
}

QPixmap KCronIcons::application(KCronIcons::IconSize size) {
	return getIcon(QLatin1String( "kcron" ), size);
}

QPixmap KCronIcons::variable(KCronIcons::IconSize size) {
	return getIcon(QLatin1String( "text" ), size);
}

QPixmap KCronIcons::task(KCronIcons::IconSize size) {
	return getIcon(QLatin1String( "system-run" ), size);
}

QPixmap KCronIcons::information(KCronIcons::IconSize size) {
	return getIcon(QLatin1String( "dialog-information" ), size);
}

QPixmap KCronIcons::error(KCronIcons::IconSize size) {
	return getIcon(QLatin1String( "dialog-error" ), size);
}
