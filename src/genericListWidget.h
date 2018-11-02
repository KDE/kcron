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

#include "cthost.h"

class GenericListWidgetPrivate;
class QKeyEvent;
class QAction;

class CrontabWidget;

/**
 * GenericListWidget
 */
class GenericListWidget : public QWidget {
	Q_OBJECT
public:

	GenericListWidget(CrontabWidget* crontabWidget, const QString& label, const QIcon& icon);
	
	~GenericListWidget();

	QTreeWidget* treeWidget() const;
	
	CTHost* ctHost() const;
	
	CrontabWidget* crontabWidget() const;
	
	void resizeColumnContents();
	
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

protected Q_SLOTS:
	virtual void modifySelection(QTreeWidgetItem* item, int position) = 0;
	
	virtual void deleteSelection() = 0;
	
protected:
	void removeAll();
	
	QTreeWidgetItem* firstSelected() const;
	
	QAction* createSeparator();
	
	void addRightAction(QAction* action, const QObject* receiver, const char* member);
	void addRightStretch();
	
	void setActionEnabled(QAction* action, bool enabled);

private:
	GenericListWidgetPrivate* const d;
	
};

#endif // GENERIC_LIST_WIDGET_H
