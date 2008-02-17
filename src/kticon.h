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

class KTIcon {

public:

	enum IconSize {Small,Normal,Large};

	static QPixmap application(KTIcon::IconSize size);
	static QPixmap system(KTIcon::IconSize size);
	static QPixmap user(KTIcon::IconSize size);
	static QPixmap variables(KTIcon::IconSize size);
	static QPixmap variable(KTIcon::IconSize size);
	static QPixmap mail(KTIcon::IconSize size);
	static QPixmap shell(KTIcon::IconSize size);
	static QPixmap home(KTIcon::IconSize size);
	static QPixmap path(KTIcon::IconSize size);
	static QPixmap tasks(KTIcon::IconSize size);
	static QPixmap task(KTIcon::IconSize size);
	
	static QPixmap information(KTIcon::IconSize size);
	static QPixmap error(KTIcon::IconSize size);

	static QPixmap getIcon(const QString&name, KTIcon::IconSize size);
private:

};

#endif // KTICON_H
