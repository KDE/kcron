/*
    KT task editor window header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTextEdit>

#include <KTitleWidget>

class QCheckBox;
class QGridLayout;
class QHBoxLayout;

class KUrlRequester;

class CTTask;

class CrontabWidget;

class SetOrClearAllButton : public QPushButton
{
public:
    enum Status { SET_ALL, CLEAR_ALL };

    SetOrClearAllButton(QWidget *parent, SetOrClearAllButton::Status status);

    void setStatus(SetOrClearAllButton::Status status);

    bool isSetAll();

    bool isClearAll();

private:
    SetOrClearAllButton::Status currentStatus;
};

class NumberPushButton : public QPushButton
{
public:
    explicit NumberPushButton(QWidget *parent);
    NumberPushButton(bool digitMode, QWidget *parent);

    void updatePalette();

    bool event(QEvent *e) override;

    void paintEvent(QPaintEvent *) override;

    bool mIsDirty;
    QPalette mPalSelected;
    QPalette mPalNormal;
};

/**
 * Task editor window.
 */
class TaskEditorDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Initialize from CTTask.
     */
    explicit TaskEditorDialog(CTTask *ctTask, const QString &caption, CrontabWidget *crontabWidget);

    /**
     * Destroy.
     */
    ~TaskEditorDialog();

private Q_SLOTS:

    /**
     * Control the task title bar.
     */
    void setupTitleWidget(const QString &comment = QLatin1String(""), KTitleWidget::MessageType = KTitleWidget::PlainMessage);

    /**
     * Enable checkbox has changed
     */
    void slotEnabledChanged();

    /**
     * Reboot checkbox has changed
     */
    void slotRebootChanged();

    /**
     * Daily checkbox has been changed.
     */
    void slotDailyChanged();

    /**
     * Apply changes and close.
     */
    void slotOK();

    /**
     * Run the wizard.
     */
    void slotWizard();

    /**
     * Set or clear all month checkboxes
     */
    void slotAllMonths();

    /**
     * A month checkbox has changed
     */
    void slotMonthChanged();

    /**
     * Set or clear all day of month checkboxes
     */
    void slotAllDaysOfMonth();

    /**
     * A day of month checkbox has changed
     */
    void slotDayOfMonthChanged();

    /**
     * Set or clear all day of week checkboxes
     */
    void slotAllDaysOfWeek();

    /**
     * A day of week checkbox has changed
     */
    void slotDayOfWeekChanged();

    /**
     * Set or clear all hour checkboxes
     */
    void slotAllHours();

    /**
     * An hour checkbox has changed
     */
    void slotHourChanged();

    void slotMinutesPreselection(int index);

    /**
     * A minute checkbox has changed
     */
    void slotMinuteChanged();

private:
    NumberPushButton *createHourButton(QGroupBox *hoursGroup, int hour);
    QGroupBox *createHoursGroup(QWidget *mainWidget);

    NumberPushButton *createMinuteButton(int minuteIndex);
    void createMinutesGroup(QWidget *mainWidget);

    /**
     * Returns true if there is no checked minute in the hidden minuteButton
     */
    bool canReduceMinutesGroup();
    void emptyMinutesGroup();
    void reduceMinutesGroup();
    void increaseMinutesGroup();

    QGroupBox *createMonthsGroup(QWidget *mainWidget);

    QGroupBox *createDaysOfMonthGroup(QWidget *mainWidget);
    QGroupBox *createDaysOfWeekGroup(QWidget *mainWidget);

    bool checkCommand();

    void defineCommandIcon();

    bool isEveryDay();

    /**
     * Task.
     */
    CTTask *mCtTask = nullptr;

    CrontabWidget *mCrontabWidget = nullptr;

    // Widgets.

    KTitleWidget *mTitleWidget = nullptr;
    QPushButton *mOkButton = nullptr;

    QComboBox *mUserCombo = nullptr;

    QTextEdit *mLeComment = nullptr;

    QLabel *mCommandIcon = nullptr;
    QPixmap mMissingCommandPixmap;
    KUrlRequester *mCommand = nullptr;

    QCheckBox *mChkEnabled = nullptr;
    QCheckBox *mChkReboot = nullptr;
    QCheckBox *mCbEveryDay = nullptr;

    QGroupBox *mBgMonth = nullptr;
    NumberPushButton *mMonthButtons[13]; // The index 0 is not used
    SetOrClearAllButton *mAllMonths = nullptr;

    QGroupBox *mBgDayOfMonth;
    NumberPushButton *mDayOfMonthButtons[32]; // The index 0 is not used
    SetOrClearAllButton *mAllDaysOfMonth = nullptr;

    QGroupBox *mBgDayOfWeek = nullptr;
    NumberPushButton *mDayOfWeekButtons[8]; // The index 0 is not used
    SetOrClearAllButton *mAllDaysOfWeek = nullptr;

    QGroupBox *mHoursGroup = nullptr;
    QLabel *mMorningLabel = nullptr;
    QLabel *mAfternoonLabel = nullptr;
    QPushButton *mHourButtons[24];
    SetOrClearAllButton *mAllHours = nullptr;

    QGroupBox *mMinutesGroup = nullptr;
    QGridLayout *mMinutesLayout = nullptr;
    QPushButton *mMinuteButtons[60];

    QHBoxLayout *mMinutesPreselectionLayout = nullptr;
    QComboBox *mMinutesPreselection = nullptr;

    static const int minuteTotal = 59; // or 55 or 59

    static const int minutePerColumn = 12; // or 30 or 12

    static const int reducedMinuteStep = 5;

    QStringList mSpecialValidCommands;
};

