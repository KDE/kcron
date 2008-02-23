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

#include "kticon.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"
#include "ktprint.h"

#include "logging.h"

class GenericListWidgetPrivate {

public:

	CTHost* ctHost;

	QTreeWidget* treeWidget;

};

/**
 * Construct tasks folder from branch.
 */
GenericListWidget::GenericListWidget(QWidget* parent, CTHost* ctHost, const QString& label, const QPixmap& icon) :
	QWidget(parent), d(new GenericListWidgetPrivate()) {

	QVBoxLayout* layout = new QVBoxLayout(this);

	d->ctHost = ctHost;

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

CTHost* GenericListWidget::ctHost() const {
	return d->ctHost;
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
