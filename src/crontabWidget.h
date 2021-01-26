/*
    KT main view header.
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CRONTABWIDGET_H
#define CRONTABWIDGET_H

#include <QWidget>

#include "tasksWidget.h"
#include "variablesWidget.h"

class QHBoxLayout;

class CTHost;
class CTCron;
class QRadioButton;
class QComboBox;
class CTGlobalCron;

/**
 * Main GUI view of the crontab entries.
 */
class CrontabWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * Initializes view.
     */
    explicit CrontabWidget(QWidget *parent, CTHost *ctHost);

    /**
     * Destructor.
     */
    ~CrontabWidget() override;

    TasksWidget *tasksWidget() const;

    VariablesWidget *variablesWidget() const;

    CTHost *ctHost() const;

    CTCron *currentCron() const;

    QList<QAction *> cutCopyPasteActions();

public Q_SLOTS:

    /**
     * Copies variables and/or tasks.
     */
    void copy();

    void cut();

    /**
     * Pastes variables and/or tasks from the clipboard.
     */
    void paste();

    /**
     * Print crontab.
     */
    void print();

protected Q_SLOTS:

    void refreshCron();

    void checkOtherUsers();

private:
    /**
     * Enables/disables paste button
     */
    void togglePasteAction(bool enabled);

    /**
     * Enables/disables modification buttons
     */
    void toggleModificationActions(bool enabled);

    /**
     * Enables/disables new entry actions
     */
    void toggleNewEntryActions(bool enabled);

    /**
     * Initialize actions.
     */
    void setupActions();

    /**
     * Initialize view from underlying objects.
     */
    void initialize();

    QHBoxLayout *createCronSelector();

    bool hasClipboardContent();

    /**
     * The application.
     */
    CTHost *mCtHost = nullptr;

    /**
     * Tree view of the crontab tasks.
     */
    TasksWidget *mTasksWidget = nullptr;

    /**
     * Tree view of the crontab tasks.
     */
    VariablesWidget *mVariablesWidget = nullptr;

    QAction *mCutAction = nullptr;
    QAction *mCopyAction = nullptr;
    QAction *mPasteAction = nullptr;

    /**
     * Clipboard tasks.
     */
    QList<CTTask *> mClipboardTasks;

    /**
     * Clipboard variable.
     */
    QList<CTVariable *> mClipboardVariables;

    QRadioButton *mCurrentUserCronRadio = nullptr;
    QRadioButton *mSystemCronRadio = nullptr;
    QRadioButton *mOtherUserCronRadio = nullptr;

    QComboBox *mOtherUsers = nullptr;

    /**
     * Pointer to the pseudo Global Cron object
     */
    CTGlobalCron *mCtGlobalCron = nullptr;
};

#endif // CRONTABWIDGET_H
