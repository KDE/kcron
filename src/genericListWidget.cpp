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
#include <QVBoxLayout>
#include <QPixmap>
#include <QKeyEvent>
#include <QAction>

#include <KLocalizedString>
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
	
	QVBoxLayout* actionsLayout;

};

/**
 * Construct tasks folder from branch.
 */
GenericListWidget::GenericListWidget(CrontabWidget* crontabWidget, const QString& label, const QPixmap& icon) :
	QWidget(crontabWidget), d(new GenericListWidgetPrivate()) {

	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	d->crontabWidget = crontabWidget;

	// Label layout
	QHBoxLayout* labelLayout = new QHBoxLayout();

	QLabel* tasksIcon = new QLabel(this);
	tasksIcon->setPixmap(icon);
	labelLayout->addWidget(tasksIcon);

	QLabel* tasksLabel = new QLabel(label, this);
	labelLayout->addWidget(tasksLabel, 1, Qt::AlignLeft);

	mainLayout->addLayout(labelLayout);

	
	// Tree layout
	QHBoxLayout* treeLayout = new QHBoxLayout();
	
	d->treeWidget = new QTreeWidget(this);

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

	treeLayout->addWidget(d->treeWidget);
	
	d->actionsLayout = new QVBoxLayout();
	
	treeLayout->addLayout(d->actionsLayout);

	mainLayout->addLayout(treeLayout);

	logDebug() << "Generic list created" << endl;
	connect(treeWidget(), SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(modifySelection(QTreeWidgetItem*,int)));

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

QAction* GenericListWidget::createSeparator() {
	QAction* action = new QAction(this);
	action->setSeparator(true);

	return action;
}

void GenericListWidget::addRightAction(QAction* action, const QObject* receiver, const char* member) {
	QPushButton* button = new QPushButton(action->text(), this);
	button->setIcon(action->icon());
	button->setWhatsThis(action->whatsThis());
	button->setToolTip(action->toolTip());

	d->actionsLayout->addWidget(button);
	
	button->addAction(action);
	
	connect(button, SIGNAL(clicked(bool)), receiver, member);
	connect(action, SIGNAL(triggered(bool)), receiver, member);
}

void GenericListWidget::addRightStretch() {
	d->actionsLayout->addStretch(1);
}

void GenericListWidget::setActionEnabled(QAction* action, bool enabled) {
	foreach(QWidget* widget, action->associatedWidgets()) {
		
		//Only change status of associated Buttons
		QPushButton* button = qobject_cast<QPushButton*>(widget);
		if (button!=NULL) {
			button->setEnabled(enabled);
		}
	}
	
	action->setEnabled(enabled);
}
