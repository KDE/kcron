/*
    KT task editor window implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "taskEditorDialog.h"

#include <QCheckBox>
#include <QEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>

#include <QLocale>

#include <QStyleOption>
#include <QStylePainter>

#include <KAcceleratorManager>
#include <KLocalizedString>
#include <KStandardShortcut>
#include <QDialogButtonBox>
#include <QPushButton>
#include <kurlrequester.h>

#include "cttask.h"
#include "kcm_cron_debug.h"

#include "crontabWidget.h"

#include "kcronHelper.h"

/**
 * TaskEditorDialog class implementation
 */

TaskEditorDialog::TaskEditorDialog(CTTask *_ctTask, const QString &_caption, CrontabWidget *_crontabWidget)
    : QDialog(_crontabWidget)
{
    setModal(true);

    // window
    setWindowIcon(QIcon::fromTheme(QStringLiteral("kcron")));
    setWindowTitle(_caption);

    mCtTask = _ctTask;
    mCrontabWidget = _crontabWidget;

    auto main = new QWidget(this);
    auto mainLayout = new QVBoxLayout(main);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);

    auto dialogLayout = new QVBoxLayout();
    dialogLayout->addWidget(main);
    dialogLayout->addWidget(buttonBox);
    setLayout(dialogLayout);

    // top title widget
    mTitleWidget = new KTitleWidget(main);
    mTitleWidget->setText(i18n("Add or modify a scheduled task"));
    setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));

    mainLayout->addWidget(mTitleWidget);

    auto commandConfigurationLayout = new QGridLayout();
    mainLayout->addLayout(commandConfigurationLayout);

    // command
    auto labCommand = new QLabel(i18n("&Command:"), main);
    commandConfigurationLayout->addWidget(labCommand, 0, 0);

    auto commandLayout = new QHBoxLayout();
    mCommandIcon = new QLabel(main);
    mMissingCommandPixmap = QIcon::fromTheme(QStringLiteral("image-missing")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this));
    commandLayout->addWidget(mCommandIcon);

    mCommand = new KUrlRequester(main);
    labCommand->setBuddy(mCommand);
    commandLayout->addWidget(mCommand);

    mCommand->setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
    mCommand->setUrl(QUrl::fromLocalFile(mCtTask->command));

    // Initialize special valid commands
    mSpecialValidCommands << QStringLiteral("cd");

    commandConfigurationLayout->addLayout(commandLayout, 0, 1);

    // User Combo
    auto userLabel = new QLabel(i18n("&Run as:"), main);
    commandConfigurationLayout->addWidget(userLabel, 1, 0);

    mUserCombo = new QComboBox(main);

    userLabel->setBuddy(mUserCombo);
    commandConfigurationLayout->addWidget(mUserCombo, 1, 1);

    // When in multiuser (system) mode, a user column is required.
    if (mCrontabWidget->tasksWidget()->needUserColumn()) {
        KCronHelper::initUserCombo(mUserCombo, mCrontabWidget, mCtTask->userLogin);
    } else {
        userLabel->hide();
        mUserCombo->hide();
    }

    // comment
    auto labComment = new QLabel(i18n("Co&mment:"), main);
    commandConfigurationLayout->addWidget(labComment, 2, 0, Qt::AlignTop);

    mLeComment = KCronHelper::createCommentEdit(main);
    labComment->setBuddy(mLeComment);
    commandConfigurationLayout->addWidget(mLeComment, 2, 1);

    mLeComment->setText(mCtTask->comment);

    auto checkboxesLayout = new QHBoxLayout();
    mainLayout->addLayout(checkboxesLayout);

    // enabled
    mChkEnabled = new QCheckBox(i18n("&Enable this task"), main);
    mChkEnabled->setChecked(mCtTask->enabled);
    checkboxesLayout->addWidget(mChkEnabled);

    // @reboot
    mChkReboot = new QCheckBox(i18n("Run at system &bootup"), main);
    mChkReboot->setChecked(mCtTask->reboot);
    checkboxesLayout->addWidget(mChkReboot);

    // Every day
    bool everyDay = isEveryDay();
    mCbEveryDay = new QCheckBox(i18n("Run &every day"), main);
    mCbEveryDay->setChecked(everyDay);
    checkboxesLayout->addWidget(mCbEveryDay);

    auto schedulingLayout = new QHBoxLayout();
    mainLayout->addLayout(schedulingLayout);

    auto monthLayout = new QVBoxLayout();
    schedulingLayout->addLayout(monthLayout);

    // months
    mBgMonth = createMonthsGroup(main);
    monthLayout->addWidget(mBgMonth);
    monthLayout->addStretch(1);

    auto v1 = new QVBoxLayout();
    schedulingLayout->addLayout(v1);

    // days of the month
    mBgDayOfMonth = createDaysOfMonthGroup(main);
    v1->addWidget(mBgDayOfMonth);

    // days of the week
    mBgDayOfWeek = createDaysOfWeekGroup(main);
    v1->addWidget(mBgDayOfWeek);

    v1->addStretch(1);

    auto v2 = new QVBoxLayout();
    schedulingLayout->addLayout(v2);

    mHoursGroup = createHoursGroup(main);
    v2->addWidget(mHoursGroup);

    createMinutesGroup(main);
    v2->addWidget(mMinutesGroup);

    v2->addStretch(1);

    // schedulingLayout->addStretch(1);

    mCommand->setFocus();

    connect(mCommand, &KUrlRequester::textChanged, this, &TaskEditorDialog::slotWizard);

    connect(mChkEnabled, &QCheckBox::clicked, this, &TaskEditorDialog::slotEnabledChanged);
    connect(mChkEnabled, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

    connect(mChkReboot, &QCheckBox::clicked, this, &TaskEditorDialog::slotRebootChanged);
    connect(mChkReboot, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

    connect(mCbEveryDay, &QCheckBox::clicked, this, &TaskEditorDialog::slotDailyChanged);
    connect(mCbEveryDay, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &TaskEditorDialog::slotOK);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    if (!mChkEnabled->isChecked()) {
        slotEnabledChanged();
    } else if (mChkReboot->isChecked()) {
        slotRebootChanged();
    } else if (mCbEveryDay->isChecked()) {
        slotDailyChanged();
    }

    slotMonthChanged();
    slotDayOfMonthChanged();
    slotDayOfWeekChanged();
    slotHourChanged();
    slotMinuteChanged();

    slotWizard();
}

TaskEditorDialog::~TaskEditorDialog()
{
}

bool TaskEditorDialog::isEveryDay()
{
    for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
        if (!mCtTask->dayOfWeek.isEnabled(dw)) {
            return false;
        }
    }

    for (int mo = mCtTask->month.minimum(); mo <= mCtTask->month.maximum(); mo++) {
        if (!mCtTask->month.isEnabled(mo)) {
            return false;
        }
    }

    for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
        if (!mCtTask->dayOfMonth.isEnabled(dm)) {
            return false;
        }
    }

    return true;
}

QGroupBox *TaskEditorDialog::createDaysOfMonthGroup(QWidget *main)
{
    auto daysOfMonthGroup = new QGroupBox(i18n("Days of Month"), main);
    auto daysOfMonthLayout = new QGridLayout(daysOfMonthGroup);

    int dm = CTDayOfMonth::MINIMUM;
    for (int row = 0; row < 5; ++row) {
        for (int column = 0; column < 7; ++column) {
            auto day = new NumberPushButton(true, daysOfMonthGroup);
            day->setText(QString::number(dm));
            day->setCheckable(true);
            day->setChecked(mCtTask->dayOfMonth.isEnabled(dm));
            mDayOfMonthButtons[dm] = day;

            connect(mDayOfMonthButtons[dm], &QAbstractButton::clicked, this, &TaskEditorDialog::slotDayOfMonthChanged);
            connect(mDayOfMonthButtons[dm], &QAbstractButton::clicked, this, &TaskEditorDialog::slotWizard);

            daysOfMonthLayout->addWidget(day, row, column);

            if (dm == CTDayOfMonth::MAXIMUM) {
                break;
                break;
            }

            dm++;
        }
    }

    mAllDaysOfMonth = new SetOrClearAllButton(daysOfMonthGroup, SetOrClearAllButton::SET_ALL);
    daysOfMonthLayout->addWidget(mAllDaysOfMonth, 4, 3, 1, 4);

    connect(mAllDaysOfMonth, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllDaysOfMonth);
    connect(mAllDaysOfMonth, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

    return daysOfMonthGroup;
}

QGroupBox *TaskEditorDialog::createMonthsGroup(QWidget *main)
{
    auto monthsGroup = new QGroupBox(i18n("Months"), main);
    auto monthsLayout = new QGridLayout(monthsGroup);

    int column = 0;
    int row = 0;

    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        mMonthButtons[mo] = new NumberPushButton(monthsGroup);
        mMonthButtons[mo]->setText(mCtTask->month.getName(mo));
        mMonthButtons[mo]->setCheckable(true);
        mMonthButtons[mo]->setChecked(mCtTask->month.isEnabled(mo));

        monthsLayout->addWidget(mMonthButtons[mo], row, column);

        connect(mMonthButtons[mo], &QAbstractButton::clicked, this, &TaskEditorDialog::slotMonthChanged);
        connect(mMonthButtons[mo], &QAbstractButton::clicked, this, &TaskEditorDialog::slotWizard);

        if (column == 1) {
            column = 0;
            row++;
        } else {
            column = 1;
        }
    }

    mAllMonths = new SetOrClearAllButton(monthsGroup, SetOrClearAllButton::SET_ALL);
    monthsLayout->addWidget(mAllMonths, row, 0, 1, 2);

    connect(mAllMonths, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllMonths);
    connect(mAllMonths, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

    return monthsGroup;
}

QGroupBox *TaskEditorDialog::createDaysOfWeekGroup(QWidget *main)
{
    auto daysOfWeekGroup = new QGroupBox(i18n("Days of Week"), main);
    auto daysOfWeekLayout = new QGridLayout(daysOfWeekGroup);

    int column = 0;
    int row = 0;
    for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
        mDayOfWeekButtons[dw] = new NumberPushButton(daysOfWeekGroup);
        mDayOfWeekButtons[dw]->setText(mCtTask->dayOfWeek.getName(dw));
        mDayOfWeekButtons[dw]->setCheckable(true);
        mDayOfWeekButtons[dw]->setChecked(mCtTask->dayOfWeek.isEnabled(dw));
        daysOfWeekLayout->addWidget(mDayOfWeekButtons[dw], row, column);

        connect(mDayOfWeekButtons[dw], &QAbstractButton::clicked, this, &TaskEditorDialog::slotDayOfWeekChanged);
        connect(mDayOfWeekButtons[dw], &QAbstractButton::clicked, this, &TaskEditorDialog::slotWizard);

        if (column == 1) {
            column = 0;
            row++;
        } else {
            column = 1;
        }
    }

    mAllDaysOfWeek = new SetOrClearAllButton(daysOfWeekGroup, SetOrClearAllButton::SET_ALL);
    daysOfWeekLayout->addWidget(mAllDaysOfWeek);

    connect(mAllDaysOfWeek, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllDaysOfWeek);
    connect(mAllDaysOfWeek, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

    return daysOfWeekGroup;
}

bool TaskEditorDialog::canReduceMinutesGroup()
{
    for (int minuteIndex = 0; minuteIndex <= minuteTotal; ++minuteIndex) {
        if (minuteIndex % reducedMinuteStep != 0) {
            if (mMinuteButtons[minuteIndex]->isChecked()) {
                return false;
            }
        }
    }

    return true;
}

void TaskEditorDialog::emptyMinutesGroup()
{
    qCDebug(KCM_CRON_LOG) << "Empty minutes layout";

    for (int minuteIndex = 0; minuteIndex <= minuteTotal; ++minuteIndex) {
        mMinutesLayout->removeWidget(mMinuteButtons[minuteIndex]);
        mMinuteButtons[minuteIndex]->hide();
        qCDebug(KCM_CRON_LOG) << "Layout count" << mMinutesLayout->count();
    }

    mMinutesLayout->removeItem(mMinutesPreselectionLayout);
}

void TaskEditorDialog::increaseMinutesGroup()
{
    /*
     * Test if the increase is already done. If this is the case, no need to redo it
     *
     * We test :
     * minutesButtons[1] will be hidden because 1%reducedMinuteStep != 0
     */
    /*
    if (minuteButtons[1]->isHidden() == false)
        return;
    */
    emptyMinutesGroup();

    qCDebug(KCM_CRON_LOG) << "Show all minutes";

    int minuteIndex = 0;
    for (int row = 0; row < (minuteTotal + 1) / minutePerColumn; ++row) {
        for (int column = 0; column < minutePerColumn; ++column) {
            mMinutesLayout->addWidget(mMinuteButtons[minuteIndex], row, column);
            mMinuteButtons[minuteIndex]->show();
            minuteIndex++;
        }
    }

    mMinutesLayout->addLayout(mMinutesPreselectionLayout, ((minuteTotal + 1) / minutePerColumn), 0, 1, minutePerColumn);
    mMinutesLayout->invalidate();
    this->resize(sizeHint());
}

void TaskEditorDialog::reduceMinutesGroup()
{
    qCDebug(KCM_CRON_LOG) << "Reducing view";

    emptyMinutesGroup();

    int nextRow = 0;
    int nextColumn = 0;

    for (int minuteIndex = 0; minuteIndex <= minuteTotal; ++minuteIndex) {
        if (minuteIndex % reducedMinuteStep == 0) {
            mMinutesLayout->addWidget(mMinuteButtons[minuteIndex], nextRow, nextColumn);
            mMinuteButtons[minuteIndex]->show();

            nextColumn++;
            if (nextColumn == 6) {
                nextColumn = 0;
                nextRow = 1;
            }
        } else {
            qCDebug(KCM_CRON_LOG) << "Reducing id" << minuteIndex;
            mCtTask->minute.setEnabled(minuteIndex, false);
            mMinuteButtons[minuteIndex]->setChecked(false);
        }
    }

    mMinutesLayout->addLayout(mMinutesPreselectionLayout, 2, 0, 1, 6);
    mMinutesLayout->invalidate();
    this->resize(sizeHint());
}

NumberPushButton *TaskEditorDialog::createMinuteButton(int minuteIndex)
{
    auto minuteButton = new NumberPushButton(true, mMinutesGroup);
    minuteButton->setText(QString::number(minuteIndex));
    minuteButton->setCheckable(true);
    minuteButton->setChecked(mCtTask->minute.isEnabled(minuteIndex));

    connect(minuteButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotMinuteChanged);
    connect(minuteButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotWizard);

    return minuteButton;
}

void TaskEditorDialog::createMinutesGroup(QWidget *main)
{
    qCDebug(KCM_CRON_LOG) << "Creating minutes group";

    mMinutesGroup = new QGroupBox(i18n("Minutes"), main);

    mMinutesLayout = new QGridLayout(mMinutesGroup);

    for (int minuteIndex = 0; minuteIndex <= minuteTotal; ++minuteIndex) {
        mMinuteButtons[minuteIndex] = createMinuteButton(minuteIndex);
    }

    mMinutesPreselectionLayout = new QHBoxLayout();

    auto minutesPreselectionLabel = new QLabel(i18n("Preselection:"));
    mMinutesPreselectionLayout->addWidget(minutesPreselectionLabel);

    mMinutesPreselection = new QComboBox(this);

    minutesPreselectionLabel->setBuddy(mMinutesPreselection);

    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("edit-clear-locationbar-ltr")), i18n("Clear selection"), -1);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("edit-rename")), i18n("Custom selection"), 0);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-month")), i18n("Each minute"), 1);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-week")), i18n("Every 2 minutes"), 2);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-workweek")), i18n("Every 5 minutes"), 5);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-upcoming-days")), i18n("Every 10 minutes"), 10);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-upcoming-days")), i18n("Every 15 minutes"), 15);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-day")), i18n("Every 20 minutes"), 20);
    mMinutesPreselection->addItem(QIcon::fromTheme(QStringLiteral("view-calendar-day")), i18n("Every 30 minutes"), 30);

    mMinutesPreselectionLayout->addWidget(mMinutesPreselection);

    connect(mMinutesPreselection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &TaskEditorDialog::slotMinutesPreselection);
    connect(mMinutesPreselection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &TaskEditorDialog::slotWizard);

    // First mandatory increase
    increaseMinutesGroup();

    if (canReduceMinutesGroup()) {
        reduceMinutesGroup();
    }

    qCDebug(KCM_CRON_LOG) << "Minutes group created";
}

NumberPushButton *TaskEditorDialog::createHourButton(QGroupBox *hoursGroup, int hour)
{
    auto hourButton = new NumberPushButton(true, hoursGroup);
    hourButton->setText(QString::number(hour));
    hourButton->setCheckable(true);
    hourButton->setChecked(mCtTask->hour.isEnabled(hour));

    connect(hourButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotHourChanged);
    connect(hourButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotWizard);

    return hourButton;
}

QGroupBox *TaskEditorDialog::createHoursGroup(QWidget *main)
{
    // Hide the AM/PM labels if the locale is set to 24h format.
    // 'A' or 'a' means am/pm is shown (and then 'h' uses 12-hour format)
    // but 'H' forces a 24-hour format anyway, even with am/pm shown.
    const QString str = QLocale().timeFormat();
    static bool use12Clock = str.contains(QLatin1Char('a'), Qt::CaseInsensitive) && !str.contains(QLatin1Char('H'));

    qCDebug(KCM_CRON_LOG) << "Creating hours group";
    auto hoursGroup = new QGroupBox(i18n("Hours"), main);

    auto hoursLayout = new QGridLayout(hoursGroup); // 5 x 6 (24h) // 5 x 7 (12h)

    if (use12Clock) {
        mMorningLabel = new QLabel(i18n("AM:"), this);
        mMorningLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        mMorningLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        hoursLayout->addWidget(mMorningLabel, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    }

    int hourCount = 0;
    for (int column = 0; column <= 3; ++column) {
        for (int hour = 0; hour <= 5; ++hour) {
            NumberPushButton *hourButton = createHourButton(hoursGroup, hourCount);
            mHourButtons[hourCount] = hourButton;
            // When using the 12h format, the hour buttons need to be inserted 1 column over to leave room for the AM/PM labels.
            hoursLayout->addWidget(hourButton, column, hour + (use12Clock ? 1 : 0));
            hourCount++;
        }
    }

    if (use12Clock) {
        mAfternoonLabel = new QLabel(i18n("PM:"), this);
        mAfternoonLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        mAfternoonLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        hoursLayout->addWidget(mAfternoonLabel, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);
    }

    mAllHours = new SetOrClearAllButton(this, SetOrClearAllButton::SET_ALL);
    hoursLayout->addWidget(mAllHours, 4, 0, 1, use12Clock ? 7 : 6);

    connect(mAllHours, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllHours);
    connect(mAllHours, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

    qCDebug(KCM_CRON_LOG) << "Create hours group";
    return hoursGroup;
}

void TaskEditorDialog::setupTitleWidget(const QString &comment, KTitleWidget::MessageType messageType)
{
    mTitleWidget->setComment(comment, messageType);
    if (messageType == KTitleWidget::ErrorMessage) {
        mTitleWidget->setIcon(QIcon::fromTheme(QStringLiteral("dialog-error")), KTitleWidget::ImageRight);
    } else {
        mTitleWidget->setIcon(QIcon::fromTheme(QStringLiteral("system-run")), KTitleWidget::ImageRight);
    }
}

void TaskEditorDialog::slotEnabledChanged()
{
    bool enabled = mChkEnabled->isChecked();
    mUserCombo->setEnabled(enabled);
    mLeComment->setEnabled(enabled);
    mCommand->setEnabled(enabled);
    mChkReboot->setEnabled(enabled);

    // if chkReboot is already checked, allow setEnabled(false) but not setEnable(true) ...
    if (!mChkReboot->isChecked() || !enabled) {
        mCbEveryDay->setEnabled(enabled);
        mHoursGroup->setEnabled(enabled);
        mMinutesGroup->setEnabled(enabled);
    }

    // if cbEveryDay is already checked, allow setEnabled(false) but not setEnable(true) ...
    if ((!mChkReboot->isChecked() && !mCbEveryDay->isChecked()) || !enabled) {
        mBgMonth->setEnabled(enabled);
        mBgDayOfMonth->setEnabled(enabled);
        mBgDayOfWeek->setEnabled(enabled);
    }
}

void TaskEditorDialog::slotRebootChanged()
{
    bool reboot = !mChkReboot->isChecked();
    mCbEveryDay->setEnabled(reboot);
    mHoursGroup->setEnabled(reboot);
    mMinutesGroup->setEnabled(reboot);

    // if cbEveryDay is already checked, bgMonth, bgDayOfMonth, bgDayOfWeek are already setEnable(flase)
    // so don't override them ! ...
    if (!mCbEveryDay->isChecked()) {
        mBgMonth->setEnabled(reboot);
        mBgDayOfMonth->setEnabled(reboot);
        mBgDayOfWeek->setEnabled(reboot);
    }
}

void TaskEditorDialog::slotDailyChanged()
{
    if (mCbEveryDay->isChecked()) {
        for (int mo = 1; mo <= 12; mo++) {
            mMonthButtons[mo]->setChecked(true);
        }
        for (int dm = 1; dm <= 31; dm++) {
            mDayOfMonthButtons[dm]->setChecked(true);
        }
        for (int dw = 1; dw <= 7; dw++) {
            mDayOfWeekButtons[dw]->setChecked(true);
        }
        mBgMonth->setEnabled(false);
        mBgDayOfMonth->setEnabled(false);
        mBgDayOfWeek->setEnabled(false);
        mAllMonths->setEnabled(false);
        mAllDaysOfMonth->setEnabled(false);
        mAllDaysOfWeek->setEnabled(false);
    } else {
        mBgMonth->setEnabled(true);
        mBgDayOfMonth->setEnabled(true);
        mBgDayOfWeek->setEnabled(true);
        mAllMonths->setEnabled(true);
        mAllDaysOfMonth->setEnabled(true);
        mAllDaysOfWeek->setEnabled(true);
    }

    slotMonthChanged();
    slotDayOfMonthChanged();
    slotDayOfWeekChanged();
}

void TaskEditorDialog::slotOK()
{
    // Make it friendly for just selecting days of the month or
    // days of the week.

    int monthDaysSelected(0);
    for (int dm = 1; dm <= 31; dm++) {
        if (mDayOfMonthButtons[dm]->isChecked()) {
            monthDaysSelected++;
        }
    }

    int weekDaysSelected(0);
    for (int dw = 1; dw <= 7; dw++) {
        if (mDayOfWeekButtons[dw]->isChecked()) {
            weekDaysSelected++;
        }
    }

    if ((monthDaysSelected == 0) && (weekDaysSelected > 0)) {
        for (int dm = 1; dm <= 31; dm++) {
            mDayOfMonthButtons[dm]->setChecked(true);
        }
    }

    if ((weekDaysSelected == 0) && (monthDaysSelected > 0)) {
        for (int dw = 1; dw <= 7; dw++) {
            mDayOfWeekButtons[dw]->setChecked(true);
        }
    }

    // save work in process
    if (mCrontabWidget->tasksWidget()->needUserColumn()) {
        mCtTask->userLogin = mUserCombo->currentText();
    }

    mCtTask->comment = mLeComment->toPlainText();
    mCtTask->command = mCommand->url().path();
    mCtTask->enabled = mChkEnabled->isChecked();
    mCtTask->reboot = mChkReboot->isChecked();

    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        mCtTask->month.setEnabled(mo, mMonthButtons[mo]->isChecked());
    }

    for (int dm = 1; dm <= 31; dm++) {
        mCtTask->dayOfMonth.setEnabled(dm, mDayOfMonthButtons[dm]->isChecked());
    }
    for (int dw = 1; dw <= 7; dw++) {
        mCtTask->dayOfWeek.setEnabled(dw, mDayOfWeekButtons[dw]->isChecked());
    }
    for (int ho = 0; ho <= 23; ho++) {
        mCtTask->hour.setEnabled(ho, mHourButtons[ho]->isChecked());
    }

    for (int mi = 0; mi <= minuteTotal; ++mi) {
        mCtTask->minute.setEnabled(mi, mMinuteButtons[mi]->isChecked());
    }

    accept();
}

void TaskEditorDialog::defineCommandIcon()
{
    CTTask tempTask(*mCtTask);
    tempTask.command = mCommand->url().path();

    mCommandIcon->setPixmap(tempTask.commandIcon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this)));
}

bool TaskEditorDialog::checkCommand()
{
    CTTask tempTask(*mCtTask);
    tempTask.command = mCommand->url().path();

    QPair<QString, bool> commandQuoted = tempTask.unQuoteCommand();

    if (commandQuoted.first.isEmpty()) {
        setupTitleWidget(i18n("<i>Please type a valid command line...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        mCommand->setFocus();
        mCommandIcon->setPixmap(mMissingCommandPixmap);

        return false;
    }

    QStringList pathCommand = tempTask.separatePathCommand(commandQuoted.first, commandQuoted.second);
    if (pathCommand.isEmpty()) {
        setupTitleWidget(i18n("<i>Please type a valid command line...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        mCommand->setFocus();
        mCommandIcon->setPixmap(mMissingCommandPixmap);

        return false;
    }

    QString path = pathCommand.at(0);
    QString binaryCommand = pathCommand.at(1);

    qCDebug(KCM_CRON_LOG) << "Looking for " << binaryCommand << "in" << path;

    bool found = false;
    bool exec = false;
    if (!QStandardPaths::findExecutable(binaryCommand, QStringList() << path).isEmpty() || mSpecialValidCommands.contains(binaryCommand)) {
        found = true;
    }
    // FIXME check if actually executable
    if (found) {
        exec = true;
    }

    if (found && !exec) {
        setupTitleWidget(i18n("<i>Please select an executable program...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        mCommand->setFocus();
        mCommandIcon->setPixmap(mMissingCommandPixmap);
        return false;
    }

    if (!found) {
        setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        mCommand->setFocus();
        mCommandIcon->setPixmap(mMissingCommandPixmap);
        return false;
    }

    return true;
}

void TaskEditorDialog::slotWizard()
{
    if (!mChkEnabled->isChecked()) {
        setupTitleWidget(i18n("<i>This task is disabled.</i>"));
        mOkButton->setEnabled(true);
        mChkEnabled->setFocus();
        return;
    }

    if (mChkReboot->isChecked()) {
        setupTitleWidget(i18n("<i>This task will be run on system bootup.</i>"));
        mOkButton->setEnabled(true);
        return;
    }

    if (mCommand->url().path().isEmpty()) {
        setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        mCommand->setFocus();
        mCommandIcon->setPixmap(mMissingCommandPixmap);
        return;
    }

    bool validCommand = checkCommand();
    if (!validCommand) {
        return;
    }

    // the months
    bool valid(false);
    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        if (mMonthButtons[mo]->isChecked()) {
            valid = true;
        }
    }

    if (!valid) {
        setupTitleWidget(i18n("<i>Please select from the 'Months' section...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        if (!mCommand->hasFocus()) {
            mMonthButtons[1]->setFocus();
        }
        return;
    }

    // the days
    valid = false;
    for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
        if (mDayOfMonthButtons[dm]->isChecked()) {
            valid = true;
        }
    }
    for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
        if (mDayOfWeekButtons[dw]->isChecked()) {
            valid = true;
        }
    }

    if (!valid) {
        setupTitleWidget(i18n("<i>Please select from either the 'Days of Month' or the 'Days of Week' section...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        if (!mCommand->hasFocus()) {
            mDayOfMonthButtons[1]->setFocus();
        }
        return;
    }

    // the hours
    valid = false;
    for (int ho = 0; ho <= 23; ho++) {
        if (mHourButtons[ho]->isChecked()) {
            valid = true;
        }
    }

    if (!valid) {
        setupTitleWidget(i18n("<i>Please select from the 'Hours' section...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        if (!mCommand->hasFocus()) {
            mHourButtons[0]->setFocus();
        }
        return;
    }

    // the mins
    valid = false;
    for (int mi = 0; mi <= minuteTotal; ++mi) {
        if (mMinuteButtons[mi]->isChecked()) {
            valid = true;
        }
    }

    if (!valid) {
        setupTitleWidget(i18n("<i>Please select from the 'Minutes' section...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        if (!mCommand->hasFocus()) {
            mMinuteButtons[0]->setFocus();
        }
        return;
    }

    defineCommandIcon();
    setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));

    mOkButton->setEnabled(true);
}

void TaskEditorDialog::slotAllMonths()
{
    bool checked = false;
    if (mAllMonths->isSetAll()) {
        checked = true;
    }

    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        mMonthButtons[mo]->setChecked(checked);
    }

    slotMonthChanged();
}

void TaskEditorDialog::slotMonthChanged()
{
    bool allCleared = true;
    for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
        if (mMonthButtons[mo]->isChecked()) {
            allCleared = false;
            break;
        }
    }

    if (allCleared) {
        mAllMonths->setStatus(SetOrClearAllButton::SET_ALL);
    } else {
        mAllMonths->setStatus(SetOrClearAllButton::CLEAR_ALL);
    }
}

void TaskEditorDialog::slotAllDaysOfMonth()
{
    bool checked = false;
    if (mAllDaysOfMonth->isSetAll()) {
        checked = true;
    }

    for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
        mDayOfMonthButtons[dm]->setChecked(checked);
    }

    slotDayOfMonthChanged();
}

void TaskEditorDialog::slotDayOfMonthChanged()
{
    bool allCleared = true;
    for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
        if (mDayOfMonthButtons[dm]->isChecked()) {
            allCleared = false;
            break;
        }
    }

    if (allCleared) {
        mAllDaysOfMonth->setStatus(SetOrClearAllButton::SET_ALL);
    } else {
        mAllDaysOfMonth->setStatus(SetOrClearAllButton::CLEAR_ALL);
    }
}

void TaskEditorDialog::slotAllDaysOfWeek()
{
    if (mAllDaysOfWeek->isSetAll()) {
        for (int dw = 1; dw <= 7; dw++) {
            mDayOfWeekButtons[dw]->setChecked(true);
        }
    } else {
        for (int dw = 1; dw <= 7; dw++) {
            mDayOfWeekButtons[dw]->setChecked(false);
        }
    }
    slotDayOfWeekChanged();
}

void TaskEditorDialog::slotDayOfWeekChanged()
{
    bool allCleared = true;
    for (int dw = 1; dw <= 7; dw++) {
        if (mDayOfWeekButtons[dw]->isChecked()) {
            allCleared = false;
        }
    }
    if (allCleared) {
        mAllDaysOfWeek->setStatus(SetOrClearAllButton::SET_ALL);
    } else {
        mAllDaysOfWeek->setStatus(SetOrClearAllButton::CLEAR_ALL);
    }
}

void TaskEditorDialog::slotAllHours()
{
    if (mAllHours->isSetAll()) {
        for (int ho = 0; ho <= 23; ho++) {
            mHourButtons[ho]->setChecked(true);
        }
    } else {
        for (int ho = 0; ho <= 23; ho++) {
            mHourButtons[ho]->setChecked(false);
        }
    }
    slotHourChanged();
}

void TaskEditorDialog::slotHourChanged()
{
    bool allCleared = true;
    for (int ho = 0; ho <= 23; ho++) {
        if (mHourButtons[ho]->isChecked()) {
            allCleared = false;
        }
    }

    if (allCleared) {
        mAllHours->setStatus(SetOrClearAllButton::SET_ALL);
    } else {
        mAllHours->setStatus(SetOrClearAllButton::CLEAR_ALL);
    }
}

void TaskEditorDialog::slotMinutesPreselection(int index)
{
    QVariant itemData = mMinutesPreselection->itemData(index);
    int step = itemData.toInt();
    qCDebug(KCM_CRON_LOG) << "Selected step " << step;

    if (step == -1) {
        // Unselect everything
        for (int mi = 0; mi <= minuteTotal; ++mi) {
            mMinuteButtons[mi]->setChecked(false);
        }

        // Select Custom selection in the combo box
        for (int index = 0; index < mMinutesPreselection->count(); ++index) {
            if (mMinutesPreselection->itemData(index).toInt() == 0) {
                mMinutesPreselection->setCurrentIndex(index);
                break;
            }
        }
    } else if (step != 0) {
        for (int mi = 0; mi <= minuteTotal; ++mi) {
            if (mi % step == 0) {
                mMinuteButtons[mi]->setChecked(true);
            } else {
                mMinuteButtons[mi]->setChecked(false);
            }
        }
    }

    if (step < reducedMinuteStep && index != 0) {
        increaseMinutesGroup();
    } else {
        reduceMinutesGroup();
    }
}

void TaskEditorDialog::slotMinuteChanged()
{
    CTMinute minutes;

    for (int index = 0; index <= minuteTotal; ++index) {
        minutes.setEnabled(index, mMinuteButtons[index]->isChecked());
    }

    int period = minutes.findPeriod();

    for (int index = 0; index < mMinutesPreselection->count(); ++index) {
        if (mMinutesPreselection->itemData(index).toInt() == period) {
            mMinutesPreselection->setCurrentIndex(index);
            break;
        }
    }
}

/**
 * SetOrClearAllButton class implementation
 */

SetOrClearAllButton::SetOrClearAllButton(QWidget *parent, SetOrClearAllButton::Status status)
    : QPushButton(parent)
{
    setStatus(status);
}

void SetOrClearAllButton::setStatus(SetOrClearAllButton::Status status)
{
    currentStatus = status;

    if (currentStatus == SetOrClearAllButton::SET_ALL) {
        setText(i18n("Set All"));
    } else {
        setText(i18n("Clear All"));
    }
}

bool SetOrClearAllButton::isSetAll()
{
    return currentStatus == SetOrClearAllButton::SET_ALL;
}

bool SetOrClearAllButton::isClearAll()
{
    return currentStatus == SetOrClearAllButton::CLEAR_ALL;
}

/**
 * KTPushButton class implementation
 */

NumberPushButton::NumberPushButton(QWidget *parent)
    : QPushButton(parent)
    , mIsDirty(false)
{
    updatePalette();
}

NumberPushButton::NumberPushButton(bool digitMode, QWidget *parent)
    : QPushButton(parent)
    , mIsDirty(false)
{
    if (digitMode) {
        setFixedWidth(12 + fontMetrics().boundingRect(QStringLiteral("44")).width());
        KAcceleratorManager::setNoAccel(this);
    }
    updatePalette();
}

void NumberPushButton::updatePalette()
{
    mPalNormal = ((QWidget *)parent())->palette();
    mPalSelected = mPalNormal;
    for (int cg = (int)QPalette::Active; cg < (int)QPalette::NColorGroups; cg++) {
        mPalSelected.setColor((QPalette::ColorGroup)cg, QPalette::Button, mPalSelected.color((QPalette::ColorGroup)cg, QPalette::Highlight));
        mPalSelected.setColor((QPalette::ColorGroup)cg, QPalette::ButtonText, mPalSelected.color((QPalette::ColorGroup)cg, QPalette::HighlightedText));
    }
    mIsDirty = true;
}

bool NumberPushButton::event(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange) {
        updatePalette();
        update();
    }
    return QPushButton::event(e);
}

void NumberPushButton::paintEvent(QPaintEvent *)
{
    QStylePainter p(this);
    QStyleOptionButton option;
    initStyleOption(&option);

    if (mIsDirty || isChecked()) {
        mIsDirty = false;
        if (isChecked()) {
            option.palette = mPalSelected;
            QFont f = p.font();
            f.setBold(true);
            p.setFont(f);
        }
    }
    p.drawControl(QStyle::CE_PushButton, option);
}
