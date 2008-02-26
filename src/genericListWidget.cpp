/***************************************************************************
 *   KT list view item tasks implementation.                               *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "genericListWidget.h"

#include <QHeaderView>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>
#include <QPixmap>
#include <QKeyEvent>

#include <klocale.h>
#include <kglobalsettings.h>

#include "ctcron.h"
#include "cttask.h"

#include "crontabWidget.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"
#include "kcronIcons.h"

#include "logging.h"

class GenericListWidgetPrivate {

public:

	QTreeWidget* treeWidget;
	
	CrontabWidget* crontabWidget;

};

/**
 * Construct tasks folder from branch.
 */
GenericListWidget::GenericListWidget(CrontabWidget* crontabWidget, const QString& label, const QPixmap& icon) :
	QWidget(crontabWidget), d(new GenericListWidgetPrivate()) {

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	d->crontabWidget = crontabWidget;

	QHBoxLayout* labelLayout = new QHBoxLayout();

	QLabel* tasksIcon = new QLabel(this);
	tasksIcon->setPixmap(icon);
	labelLayout->addWidget(tasksIcon);

	QLabel* tasksLabel = new QLabel(label, this);
	labelLayout->addWidget(tasksLabel, 1, Qt::AlignLeft);

	layout->addLayout(labelLayout);

	d->treeWidget = new QTreeWidget(this);
	layout->addWidget(d->treeWidget);

	d->treeWidget->setRootIsDecorated(true);
	d->treeWidget->setAllColumnsShowFocus(true);

	d->treeWidget->header()->setSortIndicatorShown(true);
	d->treeWidget->header()->setStretchLastSection(true);
	d->treeWidget->header()->setMovable(true);

	d->treeWidget->setSortingEnabled(true);
	d->treeWidget->setAnimated(true);

	d->treeWidget->setRootIsDecorated(false);

	d->treeWidget->setAllColumnsShowFocus(true);

	d->treeWidget->setAlternatingRowColors(true);

	d->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	d->treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

	logDebug() << "Generic list created" << endl;
	connect(treeWidget(), SIGNAL(itemActivated(QTreeWidgetItem*, int)), SLOT(modifySelection(QTreeWidgetItem*, int)));

}

GenericListWidget::~GenericListWidget() {
	delete d;
}

QTreeWidget* GenericListWidget::treeWidget() const {
	return d->treeWidget;
}

CrontabWidget* GenericListWidget::crontabWidget() const {
	return d->crontabWidget;
}

CTHost* GenericListWidget::ctHost() const {
	return d->crontabWidget->ctHost();
}

void GenericListWidget::resizeColumnContents() {

	//Resize all columns except the last one (which always take the last available space)
	for (int i=0; i<d->treeWidget->columnCount()-1; ++i) {
		d->treeWidget->resizeColumnToContents(i);
	}

}

QTreeWidgetItem* GenericListWidget::firstSelected() const {
	QList<QTreeWidgetItem*> tasksItems = treeWidget()->selectedItems();
	if (tasksItems.isEmpty()) {
		return NULL;
	}

	return tasksItems.first();
}

void GenericListWidget::keyPressEvent(QKeyEvent *e) {
	if ( e->key() == Qt::Key_Delete ) {
		deleteSelection();
	}
}


void GenericListWidget::removeAll() {
	//Remove previous items
	for(int i= treeWidget()->topLevelItemCount()-1; i>=0; --i) {
		delete treeWidget()->takeTopLevelItem(i);
		
	}
}
