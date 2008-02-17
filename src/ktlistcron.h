/***************************************************************************
 *   KT list view item cron header.                                        *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef KTLISTCRON_H
#define KTLISTCRON_H

#include "ktlistitem.h"

/**
 * QListViewItem view of a CTCron.
 */
class KTListCron : public KTListItem {
public:

	/**
	 * Construct tasks or variables folder from branch.
	 */
	KTListCron(KTListItem* parent, const char* name, CTCron* _ctcron);

	/**
	 * Construct tasks or variables folder from root.
	 */
	KTListCron(Q3ListView* parent, const char* name, CTCron* _ctcron);

	/**
	 * Construct user folder from root.
	 */
	KTListCron(Q3ListView* parent, CTCron* _ctcron);

	/**
	 * Refresh.
	 */
	virtual void refresh();

	/**
	 *Print user's crontab
	 */
	virtual void print(KTPrint &printer) const;

};

#endif // KTLISTCRON_H
