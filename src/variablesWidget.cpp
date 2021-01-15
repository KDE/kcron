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
/**
 * Construct tasks folder from branch.
 */
VariablesWidget::VariablesWidget(CrontabWidget *crontabWidget)
    : GenericListWidget(crontabWidget, i18n("<b>Environment Variables</b>"), QIcon::fromTheme(QStringLiteral("text-plain")))
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
}

void VariablesWidget::modifySelection()
{
    modifySelection(firstSelectedVariableWidget(), -1);
}

void VariablesWidget::modifySelection(QTreeWidgetItem *item, int position)
{
    VariableWidget *variableWidget = static_cast<VariableWidget *>(item);

    if (variableWidget) {
        if (position == statusColumnIndex()) {
            variableWidget->toggleEnable();
            Q_EMIT variableModified(true);
        } else {
            CTVariable *variable = variableWidget->getCTVariable();
            VariableEditorDialog variableEditorDialog(variable, i18n("Modify Variable"), crontabWidget());
            int result = variableEditorDialog.exec();

            if (result == QDialog::Accepted) {
                crontabWidget()->currentCron()->modifyVariable(variable);
                variableWidget->refresh();

                Q_EMIT variableModified(true);
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
        Q_EMIT variableModified(true);
        changeCurrentSelection();
    }
}

bool VariablesWidget::needUserColumn()
{
    CTCron *currentCron = crontabWidget()->currentCron();
    if (currentCron->isMultiUserCron() && !currentCron->isSystemCron()) {
        return true;
    }

    return false;
}

int VariablesWidget::statusColumnIndex()
{
    if (needUserColumn()) {
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
        Q_EMIT variableModified(true);
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
    mNewVariableAction = new QAction(this);
    mNewVariableAction->setIcon(QIcon::fromTheme(QStringLiteral("document-new")));
    mNewVariableAction->setText(i18nc("Adds a new variable", "New &Variable..."));
    mNewVariableAction->setToolTip(i18n("Create a new variable."));
    addRightAction(mNewVariableAction, this, SLOT(createVariable()));

    mModifyAction = new QAction(this);
    mModifyAction->setText(i18n("M&odify..."));
    mModifyAction->setIcon(QIcon::fromTheme(QStringLiteral("document-open")));
    mModifyAction->setToolTip(i18n("Modify the selected variable."));
    addRightAction(mModifyAction, this, SLOT(modifySelection()));

    mDeleteAction = new QAction(this);
    mDeleteAction->setText(i18n("&Delete"));
    mDeleteAction->setIcon(QIcon::fromTheme(QStringLiteral("edit-delete")));
    mDeleteAction->setToolTip(i18n("Delete the selected variable."));
    addRightAction(mDeleteAction, this, SLOT(deleteSelection()));

    addRightStretch();
}

void VariablesWidget::prepareContextualMenu()
{
    treeWidget()->addAction(mNewVariableAction);

    treeWidget()->addAction(createSeparator());

    treeWidget()->addAction(mModifyAction);
    treeWidget()->addAction(mDeleteAction);

    treeWidget()->addAction(createSeparator());

    foreach (QAction *action, crontabWidget()->cutCopyPasteActions()) {
        treeWidget()->addAction(action);
    }
}

void VariablesWidget::toggleModificationActions(bool state)
{
    setActionEnabled(mModifyAction, state);
    setActionEnabled(mDeleteAction, state);
}

void VariablesWidget::toggleNewEntryAction(bool state)
{
    setActionEnabled(mNewVariableAction, state);
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
