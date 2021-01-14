/*
    KT list view item tasks implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "genericListWidget.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QAction>


#include "ctcron.h"
#include "cttask.h"

#include "crontabWidget.h"
#include "taskWidget.h"
#include "taskEditorDialog.h"

#include "logging.h"

/**
 * Construct tasks folder from branch.
 */
GenericListWidget::GenericListWidget(CrontabWidget *crontabWidget, const QString &label, const QIcon &icon)
    : QWidget(crontabWidget)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mCrontabWidget = crontabWidget;

    // Label layout
    QHBoxLayout *labelLayout = new QHBoxLayout();

    QLabel *tasksIcon = new QLabel(this);
    tasksIcon->setPixmap(icon.pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this)));
    labelLayout->addWidget(tasksIcon);

    QLabel *tasksLabel = new QLabel(label, this);
    labelLayout->addWidget(tasksLabel, 1, Qt::AlignLeft);

    mainLayout->addLayout(labelLayout);

    // Tree layout
    QHBoxLayout *treeLayout = new QHBoxLayout();

    mTreeWidget = new QTreeWidget(this);

    mTreeWidget->setRootIsDecorated(true);
    mTreeWidget->setAllColumnsShowFocus(true);

    mTreeWidget->header()->setSortIndicatorShown(true);
    mTreeWidget->header()->setStretchLastSection(true);
    mTreeWidget->header()->setSectionsMovable(true);

    mTreeWidget->setSortingEnabled(true);
    mTreeWidget->setAnimated(true);

    mTreeWidget->setRootIsDecorated(false);

    mTreeWidget->setAllColumnsShowFocus(true);

    mTreeWidget->setAlternatingRowColors(true);

    mTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mTreeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    treeLayout->addWidget(mTreeWidget);

    mActionsLayout = new QVBoxLayout();

    treeLayout->addLayout(mActionsLayout);

    mainLayout->addLayout(treeLayout);

    logDebug() << "Generic list created";
    connect(treeWidget(), &QTreeWidget::itemDoubleClicked, this, &GenericListWidget::modifySelection);
}

GenericListWidget::~GenericListWidget()
{
}

QTreeWidget *GenericListWidget::treeWidget() const
{
    return mTreeWidget;
}

CrontabWidget *GenericListWidget::crontabWidget() const
{
    return mCrontabWidget;
}

CTHost *GenericListWidget::ctHost() const
{
    return mCrontabWidget->ctHost();
}

void GenericListWidget::resizeColumnContents()
{
    //Resize all columns except the last one (which always take the last available space)
    for (int i = 0, total = mTreeWidget->columnCount()-1; i < total; ++i) {
        mTreeWidget->resizeColumnToContents(i);
    }
}

QTreeWidgetItem *GenericListWidget::firstSelected() const
{
    const QList<QTreeWidgetItem *> tasksItems = treeWidget()->selectedItems();
    if (tasksItems.isEmpty()) {
        return nullptr;
    }

    return tasksItems.constFirst();
}

void GenericListWidget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Delete) {
        deleteSelection();
    }
}

void GenericListWidget::removeAll()
{
    //Remove previous items
    for (int i = treeWidget()->topLevelItemCount()-1; i >= 0; --i) {
        delete treeWidget()->takeTopLevelItem(i);
    }
}

QAction *GenericListWidget::createSeparator()
{
    QAction *action = new QAction(this);
    action->setSeparator(true);

    return action;
}

void GenericListWidget::addRightAction(QAction *action, const QObject *receiver, const char *member)
{
    QPushButton *button = new QPushButton(action->text(), this);
    button->setIcon(action->icon());
    button->setWhatsThis(action->whatsThis());
    button->setToolTip(action->toolTip());

    mActionsLayout->addWidget(button);

    button->addAction(action);

    connect(button, SIGNAL(clicked(bool)), receiver, member);
    connect(action, SIGNAL(triggered(bool)), receiver, member);
}

void GenericListWidget::addRightStretch()
{
    mActionsLayout->addStretch(1);
}

void GenericListWidget::setActionEnabled(QAction *action, bool enabled)
{
    const auto associatedWidgets = action->associatedWidgets();
    for (QWidget *widget : associatedWidgets) {
        //Only change status of associated Buttons
        QPushButton *button = qobject_cast<QPushButton *>(widget);
        if (button) {
            button->setEnabled(enabled);
        }
    }

    action->setEnabled(enabled);
}
