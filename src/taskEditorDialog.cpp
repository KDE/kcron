/***************************************************************************
 *   KT task editor window implementation                                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "taskEditorDialog.h"

#include <QLabel>

#include <QLayout>
#include <QCheckBox>
#include <QPalette>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QStandardPaths>

#include <QStyleOption>
#include <QStylePainter>

#include <kacceleratormanager.h>
#include <KLocalizedString>
#include <kmessagebox.h>
#include <qpushbutton.h>
#include <kstandardshortcut.h>
#include <ktitlewidget.h>
#include <kurlrequester.h>

#include "logging.h"
#include "cttask.h"
#include "ctcron.h"
#include "cthost.h"

#include "crontabWidget.h"

#include "kcronHelper.h"

/**
 * TaskEditorDialog class implementation
 */

TaskEditorDialog::TaskEditorDialog(CTTask* _ctTask, const QString& _caption, CrontabWidget* _crontabWidget) :
	QDialog(_crontabWidget) {

	setModal(true);

	// window
	setWindowIcon(QIcon::fromTheme(QLatin1String("kcron")));
	setWindowTitle(_caption);

	ctTask = _ctTask;
	crontabWidget = _crontabWidget;

	QWidget* main = new QWidget(this);
	QVBoxLayout* mainLayout = new QVBoxLayout(main);
	mainLayout->setContentsMargins(0, 0, 0, 0);

	QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
	okButton = buttonBox->button(QDialogButtonBox::Ok);

	QVBoxLayout* dialogLayout = new QVBoxLayout();
	dialogLayout->addWidget(main);
	dialogLayout->addWidget(buttonBox);
	setLayout(dialogLayout);

	// top title widget
	titleWidget = new KTitleWidget(main);
	titleWidget->setText(i18n("Add or modify a scheduled task"));
	setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));

	mainLayout->addWidget(titleWidget);


	QGridLayout* commandConfigurationLayout = new QGridLayout();
	mainLayout->addLayout(commandConfigurationLayout);

	// command
	QLabel* labCommand = new QLabel( i18n("&Command:"), main );
	commandConfigurationLayout->addWidget(labCommand, 0, 0);

	QHBoxLayout* commandLayout = new QHBoxLayout();
	commandIcon = new QLabel(main);
	missingCommandPixmap = QIcon::fromTheme(QLatin1String("image-missing")).pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this));
	commandLayout->addWidget(commandIcon);

	command = new KUrlRequester(main);
	labCommand->setBuddy(command);
	commandLayout->addWidget(command);

	command->setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
	command->setUrl(QUrl::fromLocalFile(ctTask->command));

	//Initialize special valid commands
	specialValidCommands << QLatin1String( "cd" );

	commandConfigurationLayout->addLayout(commandLayout, 0, 1);

	// User Combo
	QLabel* userLabel = new QLabel( i18n("&Run as:"), main);
	commandConfigurationLayout->addWidget(userLabel, 1, 0);

	userCombo = new QComboBox(main);

	userLabel->setBuddy(userCombo);
	commandConfigurationLayout->addWidget(userCombo, 1, 1);

	if (crontabWidget->tasksWidget()->needUserColumn()) {
		KCronHelper::initUserCombo(userCombo, crontabWidget, ctTask->userLogin);
	} else {
		userLabel->hide();
		userCombo->hide();
	}

	// comment
	QLabel* labComment = new QLabel( i18n("Co&mment:"), main);
	commandConfigurationLayout->addWidget(labComment, 2, 0, Qt::AlignTop);

	leComment = KCronHelper::createCommentEdit(main);
	labComment->setBuddy(leComment);
	commandConfigurationLayout->addWidget(leComment, 2, 1);

	leComment->setText(ctTask->comment);

	QHBoxLayout* checkboxesLayout = new QHBoxLayout();
	mainLayout->addLayout(checkboxesLayout);

	// enabled
	chkEnabled = new QCheckBox(i18n("&Enable this task"), main);
	chkEnabled->setChecked(ctTask->enabled);
	checkboxesLayout->addWidget(chkEnabled);

	// @reboot
	chkReboot = new QCheckBox(i18n("Run at system &bootup"), main);
	chkReboot->setChecked(ctTask->reboot);
	checkboxesLayout->addWidget(chkReboot);

	// Every day
	bool everyDay = isEveryDay();
	cbEveryDay = new QCheckBox( i18n("Run &every day"), main);
	cbEveryDay->setChecked(everyDay);
	checkboxesLayout->addWidget(cbEveryDay);

	QHBoxLayout* schedulingLayout = new QHBoxLayout();
	mainLayout->addLayout(schedulingLayout);

	QVBoxLayout* monthLayout = new QVBoxLayout();
	schedulingLayout->addLayout(monthLayout);

	// months
	bgMonth = createMonthsGroup(main);
	monthLayout->addWidget(bgMonth);
	monthLayout->addStretch(1);

	QVBoxLayout* v1 = new QVBoxLayout();
	schedulingLayout->addLayout(v1);

	// days of the month
	bgDayOfMonth = createDaysOfMonthGroup(main);
	v1->addWidget(bgDayOfMonth);

	// days of the week
	bgDayOfWeek = createDaysOfWeekGroup(main);
	v1->addWidget(bgDayOfWeek);

	v1->addStretch(1);

	QVBoxLayout* v2 = new QVBoxLayout();
	schedulingLayout->addLayout(v2);

	hoursGroup = createHoursGroup(main);
	v2->addWidget(hoursGroup);

	createMinutesGroup(main);
	v2->addWidget(minutesGroup);

	v2->addStretch(1);

	//schedulingLayout->addStretch(1);

	command->setFocus();

	connect(command, &KUrlRequester::textChanged, this, &TaskEditorDialog::slotWizard);

	connect(chkEnabled, &QCheckBox::clicked, this, &TaskEditorDialog::slotEnabledChanged);
	connect(chkEnabled, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

	connect(chkReboot, &QCheckBox::clicked, this, &TaskEditorDialog::slotRebootChanged);
	connect(chkReboot, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

	connect(cbEveryDay, &QCheckBox::clicked, this, &TaskEditorDialog::slotDailyChanged);
	connect(cbEveryDay, &QCheckBox::clicked, this, &TaskEditorDialog::slotWizard);

	connect(buttonBox, &QDialogButtonBox::accepted, this, &TaskEditorDialog::slotOK);
	connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

	if (!chkEnabled->isChecked())
		slotEnabledChanged();
	else if (chkReboot->isChecked())
		slotRebootChanged();
	else if (cbEveryDay->isChecked())
		slotDailyChanged();

	slotMonthChanged();
	slotDayOfMonthChanged();
	slotDayOfWeekChanged();
	slotHourChanged();
	slotMinuteChanged();

	slotWizard();

}

TaskEditorDialog::~TaskEditorDialog() {

}

bool TaskEditorDialog::isEveryDay() {
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		if (!ctTask->dayOfWeek.isEnabled(dw))
			return false;
	}

	for (int mo = ctTask->month.minimum(); mo <= ctTask->month.maximum(); mo++) {
		if (!ctTask->month.isEnabled(mo))
			return false;
	}

	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (!ctTask->dayOfMonth.isEnabled(dm))
			return false;
	}

	return true;
}

QGroupBox* TaskEditorDialog::createDaysOfMonthGroup(QWidget* main) {

	QGroupBox* daysOfMonthGroup = new QGroupBox( i18n("Days of Month"), main);
	QGridLayout* daysOfMonthLayout = new QGridLayout(daysOfMonthGroup);

	int dm = CTDayOfMonth::MINIMUM;
	for (int row = 0; row < 5; ++row) {
		for (int column = 0; column < 7; ++column) {
			NumberPushButton* day = new NumberPushButton(true, daysOfMonthGroup);
			day->setText(QString::number(dm));
			day->setCheckable(true);
			day->setChecked(ctTask->dayOfMonth.isEnabled(dm));
			dayOfMonthButtons[dm] = day;

			connect(dayOfMonthButtons[dm], SIGNAL(clicked()), SLOT(slotDayOfMonthChanged()));
			connect(dayOfMonthButtons[dm], SIGNAL(clicked()), SLOT(slotWizard()));

			daysOfMonthLayout->addWidget(day, row, column);

			if (dm==CTDayOfMonth::MAXIMUM) {
				break;
				break;
			}

			dm++;
		}
	}


	allDaysOfMonth = new SetOrClearAllButton(daysOfMonthGroup, SetOrClearAllButton::SET_ALL);
	daysOfMonthLayout->addWidget(allDaysOfMonth, 4, 3, 1, 4);

	connect(allDaysOfMonth, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllDaysOfMonth);
	connect(allDaysOfMonth, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

	return daysOfMonthGroup;
}

QGroupBox* TaskEditorDialog::createMonthsGroup(QWidget* main) {

	QGroupBox* monthsGroup = new QGroupBox( i18n("Months"), main);
	QGridLayout* monthsLayout = new QGridLayout(monthsGroup);

	int column = 0;
	int row = 0;

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		monthButtons[mo] = new NumberPushButton(monthsGroup);
		monthButtons[mo]->setText(ctTask->month.getName(mo));
		monthButtons[mo]->setCheckable(true);
		monthButtons[mo]->setChecked(ctTask->month.isEnabled(mo));

		monthsLayout->addWidget(monthButtons[mo], row, column);

		connect(monthButtons[mo], SIGNAL(clicked()), SLOT(slotMonthChanged()));
		connect(monthButtons[mo], SIGNAL(clicked()), SLOT(slotWizard()));

		if (column == 1) {
			column = 0;
			row++;
		} else {
			column = 1;
		}
	}

	allMonths = new SetOrClearAllButton(monthsGroup, SetOrClearAllButton::SET_ALL);
	monthsLayout->addWidget(allMonths, row, 0, 1, 2);

	connect(allMonths, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllMonths);
	connect(allMonths, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

	return monthsGroup;

}

QGroupBox* TaskEditorDialog::createDaysOfWeekGroup(QWidget* main) {

	QGroupBox* daysOfWeekGroup = new QGroupBox( i18n("Days of Week"), main);
	QGridLayout* daysOfWeekLayout = new QGridLayout(daysOfWeekGroup);

	int column = 0;
	int row = 0;
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		dayOfWeekButtons[dw] = new NumberPushButton(daysOfWeekGroup);
		dayOfWeekButtons[dw]->setText(ctTask->dayOfWeek.getName(dw));
		dayOfWeekButtons[dw]->setCheckable(true);
		dayOfWeekButtons[dw]->setChecked(ctTask->dayOfWeek.isEnabled(dw));
		daysOfWeekLayout->addWidget(dayOfWeekButtons[dw], row, column);

		connect(dayOfWeekButtons[dw], SIGNAL(clicked()), SLOT(slotDayOfWeekChanged()));
		connect(dayOfWeekButtons[dw], SIGNAL(clicked()), SLOT(slotWizard()));

		if (column == 1) {
			column = 0;
			row++;
		} else {
			column = 1;
		}

	}

	allDaysOfWeek = new SetOrClearAllButton(daysOfWeekGroup, SetOrClearAllButton::SET_ALL);
	daysOfWeekLayout->addWidget(allDaysOfWeek);

	connect(allDaysOfWeek, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllDaysOfWeek);
	connect(allDaysOfWeek, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

	return daysOfWeekGroup;
}


bool TaskEditorDialog::canReduceMinutesGroup() {
	for (int minuteIndex = 0; minuteIndex<=minuteTotal; ++minuteIndex ) {
		if (minuteIndex % reducedMinuteStep != 0) {
			if (minuteButtons[minuteIndex]->isChecked() == true) {
				return false;
			}
		}

	}

	return true;
}

void TaskEditorDialog::emptyMinutesGroup() {

	logDebug() << "Empty minutes layout" << endl;

	for (int minuteIndex = 0; minuteIndex<=minuteTotal; ++minuteIndex ) {
		minutesLayout->removeWidget(minuteButtons[minuteIndex]);
		minuteButtons[minuteIndex]->hide();
		logDebug() << "Layout count" << minutesLayout->count() << endl;
	}

	minutesLayout->removeItem(minutesPreselectionLayout);
}

void TaskEditorDialog::increaseMinutesGroup() {
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

	logDebug() << "Show all minutes" << endl;

	int minuteIndex = 0;
	for (int row = 0; row < (minuteTotal+1)/minutePerColumn; ++row) {
		for (int column = 0; column < minutePerColumn; ++column) {
			minutesLayout->addWidget(minuteButtons[minuteIndex], row, column);
			minuteButtons[minuteIndex]->show();
			minuteIndex++;
		}
	}

	minutesLayout->addLayout(minutesPreselectionLayout, ((minuteTotal+1)/minutePerColumn), 0, 1, minutePerColumn);
	minutesLayout->invalidate();
	this->resize(sizeHint());

}

void TaskEditorDialog::reduceMinutesGroup() {
	logDebug() << "Reducing view" << endl;

	emptyMinutesGroup();

	int nextRow = 0;
	int nextColumn = 0;

	for (int minuteIndex = 0; minuteIndex <= minuteTotal; ++minuteIndex) {
		if (minuteIndex % reducedMinuteStep == 0) {
			minutesLayout->addWidget(minuteButtons[minuteIndex], nextRow, nextColumn);
			minuteButtons[minuteIndex]->show();

			nextColumn++;
			if (nextColumn==6) {
				nextColumn = 0;
				nextRow = 1;
			}
		}
		else {
			logDebug() << "Reducing id" << minuteIndex << endl;
			ctTask->minute.setEnabled(minuteIndex, false);
			minuteButtons[minuteIndex]->setChecked(false);

		}
	}

	minutesLayout->addLayout(minutesPreselectionLayout, 2, 0, 1, 6);
	minutesLayout->invalidate();
	this->resize(sizeHint());

}

NumberPushButton* TaskEditorDialog::createMinuteButton(int minuteIndex) {
	NumberPushButton* minuteButton = new NumberPushButton(true, minutesGroup);
	minuteButton->setText(QString::number(minuteIndex));
	minuteButton->setCheckable(true);
	minuteButton->setChecked(ctTask->minute.isEnabled(minuteIndex));

	connect(minuteButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotMinuteChanged);
	connect(minuteButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotWizard);

	return minuteButton;
}

void TaskEditorDialog::createMinutesGroup(QWidget* main) {
	logDebug() << "Creating minutes group" << endl;

	minutesGroup = new QGroupBox( i18n("Minutes"), main);

	minutesLayout = new QGridLayout(minutesGroup);

	for (int minuteIndex = 0; minuteIndex<=minuteTotal; ++minuteIndex ) {
		minuteButtons[minuteIndex] = createMinuteButton(minuteIndex);
	}

	minutesPreselectionLayout = new QHBoxLayout();

	QLabel* minutesPreselectionLabel = new QLabel(i18n("Preselection:"));
	minutesPreselectionLayout->addWidget(minutesPreselectionLabel);

	minutesPreselection = new QComboBox(this);

	minutesPreselectionLabel->setBuddy(minutesPreselection);

	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "edit-clear-locationbar-ltr" )), i18n("Clear selection"), -1);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "edit-rename" )),i18n("Custom selection"), 0);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-month" )), i18n("Each minute"), 1);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-week" )), i18n("Every 2 minutes"), 2);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-workweek" )), i18n("Every 5 minutes"), 5);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-upcoming-days" )), i18n("Every 10 minutes"), 10);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-upcoming-days" )), i18n("Every 15 minutes"), 15);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-day" )), i18n("Every 20 minutes"), 20);
	minutesPreselection->addItem(QIcon::fromTheme(QLatin1String( "view-calendar-day" )), i18n("Every 30 minutes"), 30);

	minutesPreselectionLayout->addWidget(minutesPreselection);

	connect(minutesPreselection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &TaskEditorDialog::slotMinutesPreselection);
	connect(minutesPreselection, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &TaskEditorDialog::slotWizard);

	//First mandatory increase
	increaseMinutesGroup();

	if (canReduceMinutesGroup()==true) {
		reduceMinutesGroup();
	}

	logDebug() << "Minutes group created" << endl;
}

NumberPushButton* TaskEditorDialog::createHourButton(QGroupBox* hoursGroup, int hour) {
	NumberPushButton* hourButton = new NumberPushButton(true, hoursGroup);
	hourButton->setText(QString::number(hour));
	hourButton->setCheckable(true);
	hourButton->setChecked(ctTask->hour.isEnabled(hour));

	connect(hourButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotHourChanged);
	connect(hourButton, &NumberPushButton::clicked, this, &TaskEditorDialog::slotWizard);

	return hourButton;
}

QGroupBox* TaskEditorDialog::createHoursGroup(QWidget* main) {

	logDebug() << "Creating hours group" << endl;
	QGroupBox* hoursGroup = new QGroupBox(i18n("Hours"), main);

	QGridLayout* hoursLayout = new QGridLayout(hoursGroup); //5 x 7

	morningLabel = new QLabel(i18n("AM:"), this);
	morningLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	morningLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	hoursLayout->addWidget(morningLabel, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);

	int hourCount = 0;
	for (int column = 0; column <= 3 ; ++column) {

		for (int hour = 0; hour <= 5; ++hour) {
			NumberPushButton* hourButton = createHourButton(hoursGroup, hourCount);
			hourButtons[hourCount] = hourButton;
			hoursLayout->addWidget(hourButton, column, hour+1);
			hourCount++;
		}

	}

	afternoonLabel = new QLabel( i18n("PM:"), this);
	afternoonLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	afternoonLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	hoursLayout->addWidget(afternoonLabel, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);

	allHours = new SetOrClearAllButton(this, SetOrClearAllButton::SET_ALL);
	hoursLayout->addWidget(allHours, 4, 0, 1, 7);

	connect(allHours, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotAllHours);
	connect(allHours, &SetOrClearAllButton::clicked, this, &TaskEditorDialog::slotWizard);

	logDebug() << "Create hours group" << endl;
	return hoursGroup;
}


void TaskEditorDialog::setupTitleWidget(const QString& comment, KTitleWidget::MessageType messageType) {
	titleWidget->setComment(comment, messageType);

	if (messageType == KTitleWidget::ErrorMessage)
		titleWidget->setPixmap(QIcon::fromTheme(QLatin1String("dialog-error")), KTitleWidget::ImageRight);
	else
		titleWidget->setPixmap(QIcon::fromTheme(QLatin1String("system-run")), KTitleWidget::ImageRight);

}

void TaskEditorDialog::slotEnabledChanged() {
	bool enabled = chkEnabled->isChecked();
	userCombo->setEnabled(enabled);
	leComment->setEnabled(enabled);
	command->setEnabled(enabled);
	chkReboot->setEnabled(enabled);

	// if chkReboot is already checked, allow setEnabled(false) but not setEnable(true) ...
	if (!chkReboot->isChecked() || !enabled) {
		cbEveryDay->setEnabled(enabled);
		hoursGroup->setEnabled(enabled);
		minutesGroup->setEnabled(enabled);
	}

	// if cbEveryDay is already checked, allow setEnabled(false) but not setEnable(true) ...
	if ((!chkReboot->isChecked() && !cbEveryDay->isChecked()) || !enabled) {
		bgMonth->setEnabled(enabled);
		bgDayOfMonth->setEnabled(enabled);
		bgDayOfWeek->setEnabled(enabled);
	}
}

void TaskEditorDialog::slotRebootChanged() {
	bool reboot = !chkReboot->isChecked();
	cbEveryDay->setEnabled(reboot);
	hoursGroup->setEnabled(reboot);
	minutesGroup->setEnabled(reboot);

	// if cbEveryDay is already checked, bgMonth, bgDayOfMonth, bgDayOfWeek are already setEnable(flase)
	// so don't override them ! ...
	if (!cbEveryDay->isChecked()) {
		bgMonth->setEnabled(reboot);
		bgDayOfMonth->setEnabled(reboot);
		bgDayOfWeek->setEnabled(reboot);
	}
}

void TaskEditorDialog::slotDailyChanged() {
	if (cbEveryDay->isChecked()) {
		for (int mo = 1; mo <= 12; mo++)
			monthButtons[mo]->setChecked(true);
		for (int dm = 1; dm <= 31; dm++)
			dayOfMonthButtons[dm]->setChecked(true);
		for (int dw = 1; dw <= 7; dw++)
			dayOfWeekButtons[dw]->setChecked(true);
		bgMonth->setEnabled(false);
		bgDayOfMonth->setEnabled(false);
		bgDayOfWeek->setEnabled(false);
		allMonths->setEnabled(false);
		allDaysOfMonth->setEnabled(false);
		allDaysOfWeek->setEnabled(false);
	} else {
		bgMonth->setEnabled(true);
		bgDayOfMonth->setEnabled(true);
		bgDayOfWeek->setEnabled(true);
		allMonths->setEnabled(true);
		allDaysOfMonth->setEnabled(true);
		allDaysOfWeek->setEnabled(true);
	}

	slotMonthChanged();
	slotDayOfMonthChanged();
	slotDayOfWeekChanged();
}

void TaskEditorDialog::slotOK() {
	// Make it friendly for just selecting days of the month or
	// days of the week.

	int monthDaysSelected(0);
	for (int dm = 1; dm <= 31; dm++) {
		if (dayOfMonthButtons[dm]->isChecked())
			monthDaysSelected++;
	}

	int weekDaysSelected(0);
	for (int dw = 1; dw <= 7; dw++) {
		if (dayOfWeekButtons[dw]->isChecked())
			weekDaysSelected++;
	}

	if ((monthDaysSelected == 0) && (weekDaysSelected > 0)) {
		for (int dm = 1; dm <= 31; dm++) {
			dayOfMonthButtons[dm]->setChecked(true);
		}
	}

	if ((weekDaysSelected == 0) && (monthDaysSelected > 0)) {
		for (int dw = 1; dw <= 7; dw++) {
			dayOfWeekButtons[dw]->setChecked(1);
		}
	}

	// save work in process
	if (crontabWidget->tasksWidget()->needUserColumn()) {
		ctTask->userLogin = userCombo->currentText();
	}

	ctTask->comment = leComment->toPlainText();
	ctTask->command = command->url().path();
	ctTask->enabled = chkEnabled->isChecked();
	ctTask->reboot = chkReboot->isChecked();

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		ctTask->month.setEnabled(mo, monthButtons[mo]->isChecked());
	}

	for (int dm = 1; dm <= 31; dm++) {
		ctTask->dayOfMonth.setEnabled(dm, dayOfMonthButtons[dm]->isChecked());
	}
	for (int dw = 1; dw <= 7; dw++) {
		ctTask->dayOfWeek.setEnabled(dw, dayOfWeekButtons[dw]->isChecked());
	}
	for (int ho = 0; ho <= 23; ho++) {
		ctTask->hour.setEnabled(ho, hourButtons[ho]->isChecked());
	}

	for (int mi = 0; mi <= minuteTotal; ++mi) {
		ctTask->minute.setEnabled(mi, minuteButtons[mi]->isChecked());
	}

	accept();
}


void TaskEditorDialog::defineCommandIcon() {
	CTTask tempTask(*ctTask);
	tempTask.command = command->url().path();

	commandIcon->setPixmap(tempTask.commandIcon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, 0, this)));
}


bool TaskEditorDialog::checkCommand() {
	CTTask tempTask(*ctTask);
	tempTask.command = command->url().path();

	QPair<QString, bool> commandQuoted = tempTask.unQuoteCommand();

	if (commandQuoted.first.isEmpty()) {
		setupTitleWidget(i18n("<i>Please type a valid command line...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		command->setFocus();
		commandIcon->setPixmap(missingCommandPixmap);

		return false;
	}


	QStringList pathCommand = tempTask.separatePathCommand(commandQuoted.first, commandQuoted.second);
	if (pathCommand.isEmpty()) {
		setupTitleWidget(i18n("<i>Please type a valid command line...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		command->setFocus();
		commandIcon->setPixmap(missingCommandPixmap);

		return false;
	}


	QString path = pathCommand.at(0);
	QString binaryCommand = pathCommand.at(1);

	logDebug() << "Looking for " << binaryCommand << "in" << path << endl;

	bool found = false;
	bool exec = false;
	if (!QStandardPaths::findExecutable(binaryCommand, QStringList() << path).isEmpty() || specialValidCommands.contains(binaryCommand))
		found = true;
	// FIXME check if actually executable
	if (found)
		exec = true;

	if (found && !exec) {
		setupTitleWidget(i18n("<i>Please select an executable program...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		command->setFocus();
		commandIcon->setPixmap(missingCommandPixmap);
		return false;
	}

	if (!found) {
		setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		command->setFocus();
		commandIcon->setPixmap(missingCommandPixmap);
		return false;
	}


	return true;
}

void TaskEditorDialog::slotWizard() {
	if (!chkEnabled->isChecked()) {
		setupTitleWidget(i18n("<i>This task is disabled.</i>"));
		okButton->setEnabled(true);
		chkEnabled->setFocus();
		return;
	}

	if (chkReboot->isChecked()) {
		setupTitleWidget(i18n("<i>This task will be run on system bootup.</i>"));
		okButton->setEnabled(true);
		return;
	}

	if (command->url().path().isEmpty()) {
		setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		command->setFocus();
		commandIcon->setPixmap(missingCommandPixmap);
		return;
	}

	bool validCommand = checkCommand();
	if (validCommand == false)
		return;

	// the months
	bool valid(false);
	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		if (monthButtons[mo]->isChecked())
			valid = true;
	}

	if (!valid) {
		setupTitleWidget(i18n("<i>Please select from the 'Months' section...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		if (!command->hasFocus())
			monthButtons[1]->setFocus();
		return;
	}

	// the days
	valid = false;
	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (dayOfMonthButtons[dm]->isChecked())
			valid = true;
	}
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		if (dayOfWeekButtons[dw]->isChecked())
			valid = true;
	}

	if (!valid) {
		setupTitleWidget(i18n("<i>Please select from either the 'Days of Month' or the 'Days of Week' section...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		if (!command->hasFocus())
			dayOfMonthButtons[1]->setFocus();
		return;
	}

	// the hours
	valid = false;
	for (int ho = 0; ho <= 23; ho++) {
		if (hourButtons[ho]->isChecked())
			valid = true;
	}

	if (!valid) {
		setupTitleWidget(i18n("<i>Please select from the 'Hours' section...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		if (!command->hasFocus())
			hourButtons[0]->setFocus();
		return;
	}

	// the mins
	valid = false;
	for (int mi = 0; mi <= minuteTotal; ++mi) {
		if (minuteButtons[mi]->isChecked())
			valid = true;
	}

	if (!valid) {
		setupTitleWidget(i18n("<i>Please select from the 'Minutes' section...</i>"), KTitleWidget::ErrorMessage);
		okButton->setEnabled(false);
		if (!command->hasFocus())
			minuteButtons[0]->setFocus();
		return;
	}

	defineCommandIcon();
	setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));

	okButton->setEnabled(true);

}

void TaskEditorDialog::slotAllMonths() {
	bool checked = false;
	if (allMonths->isSetAll()) {
		checked = true;
	}

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		monthButtons[mo]->setChecked(checked);
	}

	slotMonthChanged();
}

void TaskEditorDialog::slotMonthChanged() {
	bool allCleared = true;
	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		if (monthButtons[mo]->isChecked()) {
			allCleared = false;
			break;
		}
	}

	if (allCleared) {
		allMonths->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		allMonths->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void TaskEditorDialog::slotAllDaysOfMonth() {
	bool checked = false;
	if (allDaysOfMonth->isSetAll()) {
		checked = true;
	}

	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		dayOfMonthButtons[dm]->setChecked(checked);
	}

	slotDayOfMonthChanged();
}

void TaskEditorDialog::slotDayOfMonthChanged() {
	bool allCleared = true;
	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (dayOfMonthButtons[dm]->isChecked()) {
			allCleared = false;
			break;
		}
	}

	if (allCleared) {
		allDaysOfMonth->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		allDaysOfMonth->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void TaskEditorDialog::slotAllDaysOfWeek() {
	if (allDaysOfWeek->isSetAll()) {
		for (int dw = 1; dw <= 7; dw++) {
			dayOfWeekButtons[dw]->setChecked(true);
		}
	} else {
		for (int dw = 1; dw <= 7; dw++) {
			dayOfWeekButtons[dw]->setChecked(false);
		}
	}
	slotDayOfWeekChanged();
}

void TaskEditorDialog::slotDayOfWeekChanged() {
	bool allChecked = true;
	bool allCleared = true;
	for (int dw = 1; dw <= 7; dw++) {
		if (dayOfWeekButtons[dw]->isChecked()) {
			allCleared = false;
		} else {
			allChecked = false;
		}
	}
	if (allCleared) {
		allDaysOfWeek->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		allDaysOfWeek->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void TaskEditorDialog::slotAllHours() {
	if (allHours->isSetAll()) {
		for (int ho = 0; ho <= 23; ho++) {
			hourButtons[ho]->setChecked(true);
		}
	} else {
		for (int ho = 0; ho <= 23; ho++) {
			hourButtons[ho]->setChecked(false);
		}
	}
	slotHourChanged();
}

void TaskEditorDialog::slotHourChanged() {
	bool allChecked = true;
	bool allCleared = true;
	for (int ho = 0; ho <= 23; ho++) {
		if (hourButtons[ho]->isChecked()) {
			allCleared = false;
		} else {
			allChecked = false;
		}
	}

	if (allCleared) {
		allHours->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		allHours->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void TaskEditorDialog::slotMinutesPreselection(int index) {
	QVariant itemData = minutesPreselection->itemData(index);
	int step = itemData.toInt();
	logDebug() << "Selected step " << step << endl;

	if (step == -1) {
		//Unselect everything
		for (int mi = 0; mi <= minuteTotal; ++mi) {
			minuteButtons[mi]->setChecked(false);
		}

		//Select Custom selection in the combo box
		for (int index=0; index < minutesPreselection->count(); ++index) {
			if (minutesPreselection->itemData(index).toInt() == 0) {
				minutesPreselection->setCurrentIndex(index);
				break;
			}
		}
	}
	else if (step != 0) {
		for (int mi = 0; mi <= minuteTotal; ++mi) {
			if (mi % step == 0)
				minuteButtons[mi]->setChecked(true);
			else
				minuteButtons[mi]->setChecked(false);
		}
	}

	if (step<reducedMinuteStep && index != 0) {
		increaseMinutesGroup();
	}
	else {
		reduceMinutesGroup();
	}

}

void TaskEditorDialog::slotMinuteChanged() {

	CTMinute minutes;

	for(int index=0; index<=minuteTotal; ++index) {
		minutes.setEnabled(index, minuteButtons[index]->isChecked());
	}

	int period = minutes.findPeriod();

	for(int index=0; index<minutesPreselection->count(); ++index) {
		if (minutesPreselection->itemData(index).toInt() == period) {
			minutesPreselection->setCurrentIndex(index);
			break;
		}
	}
}


/**
 * SetOrClearAllButton class implementation
 */

SetOrClearAllButton::SetOrClearAllButton(QWidget * parent, SetOrClearAllButton::Status status) :
	QPushButton(parent) {

	setStatus(status);
}

void SetOrClearAllButton::setStatus(SetOrClearAllButton::Status status) {
	currentStatus = status;

	if (currentStatus == SetOrClearAllButton::SET_ALL)
		setText(i18n("Set All"));
	else
		setText(i18n("Clear All"));

}

bool SetOrClearAllButton::isSetAll() {
	return currentStatus == SetOrClearAllButton::SET_ALL;
}

bool SetOrClearAllButton::isClearAll() {
	return currentStatus == SetOrClearAllButton::CLEAR_ALL;
}

/**
 * KTPushButton class implementation
 */

NumberPushButton::NumberPushButton(QWidget * parent) :
	QPushButton(parent), isDirty(false) {
	updatePalette();
}

NumberPushButton::NumberPushButton(bool digitMode, QWidget * parent) :
	QPushButton(parent), isDirty(false) {
	if (digitMode) {
		setFixedWidth(12 + fontMetrics().width(QLatin1String( "44" )));
		KAcceleratorManager::setNoAccel(this);
	}
	updatePalette();
}

void NumberPushButton::updatePalette() {
	palNormal = ((QWidget *)parent())->palette();
	palSelected = palNormal;
	for (int cg = (int) QPalette::Active; cg < (int) QPalette::NColorGroups; cg++) {
		palSelected.setColor( (QPalette::ColorGroup)cg, QPalette::Button, palSelected.color((QPalette::ColorGroup)cg, QPalette::Highlight));
		palSelected.setColor( (QPalette::ColorGroup)cg, QPalette::ButtonText, palSelected.color((QPalette::ColorGroup)cg, QPalette::HighlightedText));
	}
	isDirty = true;
}

bool NumberPushButton::event(QEvent *e) {
	if (e->type() == QEvent::PaletteChange) {
		updatePalette();
		update();
	}
	return QPushButton::event(e);
}

void NumberPushButton::paintEvent(QPaintEvent*) {
	QStylePainter p(this);
	QStyleOptionButton option;
	initStyleOption(&option);

	if (isDirty || isChecked()) {
		isDirty = false;
		if (isChecked()) {
			option.palette = palSelected;
			QFont f=p.font();
			f.setBold(true);
			p.setFont(f);
		}
	}
	p.drawControl(QStyle::CE_PushButton, option);
}


