/*
    KT list view item tasks implementation.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "variablesWidget.h"

#include <QAction>
#include <QList>

#include <KLocalizedString>
#include <QIcon>

#include "ctcron.h"
#include "cttask.h"
#include "ctvariable.h"

#include "crontabWidget.h"
#include "variableWidget.h"
#include "variableEditorDialog.h"

#include "logging.h"

class VariablesWidgetPrivate
{
public:

    QAction *newVariableAction = nullptr;

    QAction *modifyAction = nullptr;

    QAction *deleteAction = nullptr;
};
/**
 * Construct tasks folder from branch.
 */
VariablesWidget::VariablesWidget(CrontabWidget *crontabWidget)
    : GenericListWidget(crontabWidget, i18n("<b>Environment Variables</b>"), QIcon::fromTheme(QStringLiteral("text-plain")))
    , d(new VariablesWidgetPrivate())
{
    refreshHeaders();

    treeWidget()->sortItems(0, Qt::AscendingOrder);

    setupActions();
    prepareContextualMenu();

    connect(treeWidget(), &QTreeWidget::itemSelectionChanged, this, &VariablesWidget::changeCurrentSelection);

    logDebug() << "Variables list created";
}

VariablesWidget::~VariablesWidget()
{
    delete d;
}

void VariablesWidget::modifySelection()
{
    modifySelection(firstSelectedVariableWidget(), -1);
}

void VariablesWidget::modifySelection(QTreeWidgetItem *item, int position)
{
    VariableWidget *variableWidget = static_cast<VariableWidget *>(item);

    if (variableWidget != nullptr) {
        if (position == statusColumnIndex()) {
            variableWidget->toggleEnable();
            emit variableModified(true);
        } else {
            CTVariable *variable = variableWidget->getCTVariable();
            VariableEditorDialog variableEditorDialog(variable, i18n("Modify Variable"), crontabWidget());
            int result = variableEditorDialog.exec();

            if (result == QDialog::Accepted) {
                crontabWidget()->currentCron()->modifyVariable(variable);
                variableWidget->refresh();

                emit variableModified(true);
            }
        }
    }
}

QList<VariableWidget *> VariablesWidget::selectedVariablesWidget() const
{
    QList<VariableWidget *> variablesWidget;

    const QList<QTreeWidgetItem *> variablesItems = treeWidget()->selectedItems();
    for (QTreeWidgetItem *item : variablesItems) {
        VariableWidget *variableWidget = static_cast<VariableWidget *>(item);
        variablesWidget.append(variableWidget);
    }

    return variablesWidget;
}

VariableWidget *VariablesWidget::firstSelectedVariableWidget() const
{
    QTreeWidgetItem *item = firstSelected();
    if (!item) {
        return nullptr;
    }

    return static_cast<VariableWidget *>(item);
}

void VariablesWidget::deleteSelection()
{
    QList<QTreeWidgetItem *> variablesItems = treeWidget()->selectedItems();
    bool deleteSomething = !(variablesItems.isEmpty());

    foreach (QTreeWidgetItem *item, variablesItems) {
        VariableWidget *variableWidget = static_cast<VariableWidget *>(item);

        crontabWidget()->currentCron()->removeVariable(variableWidget->getCTVariable());
        delete variableWidget->getCTVariable();
        treeWidget()->takeTopLevelItem(treeWidget()->indexOfTopLevelItem(variableWidget));
        delete variableWidget;
    }

    if (deleteSomething) {
        emit variableModified(true);
        changeCurrentSelection();
    }
}

bool VariablesWidget::needUserColumn()
{
    CTCron *currentCron = crontabWidget()->currentCron();
    if (currentCron->isMultiUserCron() == true && currentCron->isSystemCron() == false) {
        return true;
    }

    return false;
}

int VariablesWidget::statusColumnIndex()
{
    if (needUserColumn() == true) {
        return 3;
    }

    return 2;
}

void VariablesWidget::createVariable()
{
    CTVariable *variable = new CTVariable(QLatin1String(""), QLatin1String(""), crontabWidget()->currentCron()->userLogin());

    VariableEditorDialog variableEditorDialog(variable, i18n("New Variable"), crontabWidget());
    int result = variableEditorDialog.exec();

    if (result == QDialog::Accepted) {
        addVariable(variable);
        emit variableModified(true);
        changeCurrentSelection();
    } else {
        delete variable;
    }
}

void VariablesWidget::addVariable(CTVariable *variable)
{
    logDebug() << "Add a new variable";
    crontabWidget()->currentCron()->addVariable(variable);
    new VariableWidget(this, variable);

    changeCurrentSelection();
}

void VariablesWidget::refreshVariables(CTCron *cron)
{
    //Remove previous items
    removeAll();

    refreshHeaders();

    foreach (CTVariable *ctVariable, cron->variables()) {
        new VariableWidget(this, ctVariable);
    }

    resizeColumnContents();
}

void VariablesWidget::refreshHeaders()
{
    QStringList headerLabels;

    if (needUserColumn()) {
        headerLabels << i18n("User");
    }

    headerLabels << i18n("Variable");
    headerLabels << i18n("Value");
    headerLabels << i18n("Status");
    headerLabels << i18n("Comment");

    treeWidget()->setHeaderLabels(headerLabels);

    if (needUserColumn()) {
        treeWidget()->setColumnCount(5);
    } else {
        treeWidget()->setColumnCount(4);
    }
}

void VariablesWidget::setupActions()
{
    d->newVariableAction = new QAction(this);
    d->newVariableAction->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    d->newVariableAction->setText(i18nc("Adds a new variable", "New &Variable..."));
    d->newVariableAction->setToolTip(i18n("Create a new variable."));
    addRightAction(d->newVariableAction, this, SLOT(createVariable()));

    d->modifyAction = new QAction(this);
    d->modifyAction->setText(i18n("M&odify..."));
    d->modifyAction->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    d->modifyAction->setToolTip(i18n("Modify the selected variable."));
    addRightAction(d->modifyAction, this, SLOT(modifySelection()));

    d->deleteAction = new QAction(this);
    d->deleteAction->setText(i18n("&Delete"));
    d->deleteAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    d->deleteAction->setToolTip(i18n("Delete the selected variable."));
    addRightAction(d->deleteAction, this, SLOT(deleteSelection()));

    addRightStretch();
}

void VariablesWidget::prepareContextualMenu()
{
    treeWidget()->addAction(d->newVariableAction);

    treeWidget()->addAction(createSeparator());

    treeWidget()->addAction(d->modifyAction);
    treeWidget()->addAction(d->deleteAction);

    treeWidget()->addAction(createSeparator());

    foreach (QAction *action, crontabWidget()->cutCopyPasteActions()) {
        treeWidget()->addAction(action);
    }
}

void VariablesWidget::toggleModificationActions(bool state)
{
    setActionEnabled(d->modifyAction, state);
    setActionEnabled(d->deleteAction, state);
}

void VariablesWidget::toggleNewEntryAction(bool state)
{
    setActionEnabled(d->newVariableAction, state);
}

void VariablesWidget::changeCurrentSelection()
{
    logDebug() << "Change selection...";

    bool enabled;
    if (treeWidget()->selectedItems().isEmpty()) {
        enabled = false;
    } else {
        enabled = true;
    }

    toggleModificationActions(enabled);
}
