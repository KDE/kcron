/***************************************************************************
 *   KT list view item cron tasks folder.                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef GENERIC_LIST_WIDGET_H
#define GENERIC_LIST_WIDGET_H

#include <QTreeWidget>

#include "crontabPrinter.h"

#include "cthost.h"

class GenericListWidgetPrivate;
class QKeyEvent;
class CrontabWidget;

/**
 * GenericListWidget
 */
class GenericListWidget : public QWidget {
	Q_OBJECT
public:

	GenericListWidget(CrontabWidget* crontabWidget, const QString& label, const QPixmap& icon);
	
	~GenericListWidget();

	QTreeWidget* treeWidget() const;
	
	CTHost* ctHost() const;
	
	CrontabWidget* crontabWidget() const;
	
	void resizeColumnContents();
	
	virtual void keyPressEvent(QKeyEvent *e);

protected slots:
	virtual void modifySelection(QTreeWidgetItem* item, int position) = 0;
	
	virtual void deleteSelection() = 0;
	
protected:
	void removeAll();
	
	QTreeWidgetItem* firstSelected() const;
	
	GenericListWidgetPrivate* const d;
	
};

#endif // GENERIC_LIST_WIDGET_H
