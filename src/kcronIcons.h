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

#ifndef KTICON_H
#define KTICON_H

#include <QPixmap>

/**
 * Wraps all icons used by the application.
 */

class KCronIcons {

public:

	enum IconSize {Small,Normal,Large};

	static QPixmap application(KCronIcons::IconSize size);
	static QPixmap system(KCronIcons::IconSize size);
	static QPixmap user(KCronIcons::IconSize size);
	static QPixmap variable(KCronIcons::IconSize size);
	static QPixmap mail(KCronIcons::IconSize size);
	static QPixmap shell(KCronIcons::IconSize size);
	static QPixmap home(KCronIcons::IconSize size);
	static QPixmap path(KCronIcons::IconSize size);
	static QPixmap task(KCronIcons::IconSize size);
	
	static QPixmap information(KCronIcons::IconSize size);
	static QPixmap error(KCronIcons::IconSize size);

	static QPixmap getIcon(const QString&name, KCronIcons::IconSize size);
private:

};

#endif // KTICON_H
