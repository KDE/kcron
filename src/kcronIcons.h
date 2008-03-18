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

#ifndef KCRON_ICONS_H
#define KCRON_ICONS_H

#include <QPixmap>

/**
 * Wraps all icons used by the application.
 */

class KCronIcons {

public:

	enum IconSize {Small,Normal,Large};

	static QPixmap application(KCronIcons::IconSize size);

	static QPixmap variable(KCronIcons::IconSize size);
	static QPixmap task(KCronIcons::IconSize size);
	
	static QPixmap information(KCronIcons::IconSize size);
	static QPixmap error(KCronIcons::IconSize size);

private:
	static QPixmap getIcon(const QString&name, KCronIcons::IconSize size);

};

#endif // KCRON_ICONS_H
