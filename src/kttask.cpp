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

#include "kttask.h"

#include <QLabel>

#include <QFileInfo>
#include <QLayout>
#include <QCheckBox>
#include <QPainter>
#include <QPalette>
#include <QEvent>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QStyleOption>
#include <QStylePainter>

#include <kacceleratormanager.h>
#include <klocale.h>
#include <klineedit.h>
#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdialog.h>
#include <kstandardshortcut.h>
#include <kstandarddirs.h>
#include <ktitlewidget.h>
#include <kiconloader.h>
#include <kurlrequester.h> 

#include "logging.h"
#include "cttask.h"

#include "kticon.h"

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

KTPushButton::KTPushButton(QWidget * parent) :
	QPushButton(parent), isDirty(false) {
	updatePalette();
}

void KTPushButton::updatePalette() {
	palNormal = ((QWidget *)parent())->palette();
	palSelected = palNormal;
	for (int cg = (int) QPalette::Active; cg < (int) QPalette::NColorGroups; cg++) {
		palSelected.setColor((QPalette::ColorGroup)cg, QColorGroup::Button, palSelected.color((QPalette::ColorGroup)cg, QColorGroup::Highlight));
		palSelected.setColor((QPalette::ColorGroup)cg, QColorGroup::ButtonText, palSelected.color((QPalette::ColorGroup)cg, QColorGroup::HighlightedText));
	}
	isDirty = true;
}

bool KTPushButton::event(QEvent *e) {
	if (e->type() == QEvent::PaletteChange) {
		updatePalette();
		update();
	}
	return QPushButton::event(e);
}

void KTPushButton::paintEvent(QPaintEvent*) {
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

/**
 * KTTask class implementation
 */

KTTask::KTTask(CTTask* _ctTask, const QString& _caption) :
	KDialog() {

	setModal(true);

	ctTask = _ctTask;
	QWidget *main = new QWidget(this);
	setMainWidget(main);

	bool everyDay = isEveryDay();

	QVBoxLayout* ml = new QVBoxLayout(main);

	// top title widget
	titleWidget = new KTitleWidget(main);
	titleWidget->setText(i18n("Add or modify a scheduled task"));
	setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));

	ml->addWidget(titleWidget);

	QGridLayout* commandConfigurationLayout = new QGridLayout(main);
	ml->addLayout(commandConfigurationLayout);

	// command
	QLabel* labCommand = new QLabel( i18n("&Program:"), main );
	commandConfigurationLayout->addWidget(labCommand, 0, 0);

	QHBoxLayout* commandLayout = new QHBoxLayout(main);
	commandIcon = new QLabel("", main);
	commandLayout->addWidget(commandIcon);

	command = new KUrlRequester(main);
	labCommand->setBuddy(command);
	commandLayout->addWidget(command);

	command->setMode(KFile::File | KFile::ExistingOnly | KFile::LocalOnly);
	command->setPath(ctTask->command);

	commandConfigurationLayout->addLayout(commandLayout, 0, 1);

	// user
	QLabel* labUser = new QLabel( i18n("&Run as:"), main);
	commandConfigurationLayout->addWidget(labUser, 1, 0);

	leUser = new KLineEdit(main);
	labUser->setBuddy(leUser);
	commandConfigurationLayout->addWidget(leUser, 1, 1);

	if (ctTask->isSystemCrontab()) {
		leUser->setText(ctTask->user);
	} else {
		labUser->hide();
		leUser->hide();
	}

	// comment
	QLabel* labComment = new QLabel( i18n("&Comment:"), main);
	commandConfigurationLayout->addWidget(labComment, 2, 0);

	leComment = new KLineEdit(main);
	labComment->setBuddy(leComment);
	commandConfigurationLayout->addWidget(leComment, 2, 1);

	leComment->setText(ctTask->comment);

	QHBoxLayout* h3a = new QHBoxLayout();
	ml->addLayout(h3a);

	// enabled
	chkEnabled = new QCheckBox(i18n("&Enabled"), main);
	chkEnabled->setChecked(ctTask->enabled);
	h3a->addWidget(chkEnabled);

	// @reboot
	chkReboot = new QCheckBox(i18n("Run at system &bootup"), main);
	chkReboot->setChecked(ctTask->reboot);
	h3a->addWidget(chkReboot);

	// Every day
	cbEveryDay = new QCheckBox( i18n("Run &every day"), main);
	cbEveryDay->setChecked(everyDay);
	h3a->addWidget(cbEveryDay);

	QHBoxLayout* h4 = new QHBoxLayout();
	ml->addLayout(h4);

	QVBoxLayout* monthLayout = new QVBoxLayout();
	h4->addLayout(monthLayout);

	// months
	bgMonth = createMonthsGroup(main);
	monthLayout->addWidget(bgMonth);
	monthLayout->addStretch(1);

	QVBoxLayout* v1 = new QVBoxLayout();
	h4->addLayout(v1);

	// days of the month
	bgDayOfMonth = createDaysOfMonthGroup(main);
	v1->addWidget(bgDayOfMonth);

	// days of the week
	bgDayOfWeek = createDaysOfWeekGroup(main);
	v1->addWidget(bgDayOfWeek);

	v1->addStretch(1);

	QVBoxLayout* v2 = new QVBoxLayout();
	h4->addLayout(v2);

	bgHour = createHoursGroup(main);
	v2->addWidget(bgHour);

	bgMinute = createMinutesGroup(main);
	v2->addWidget(bgMinute);

	v2->addStretch(1);

	// window
	setWindowIcon(KTIcon::application(KTIcon::Small));
	setCaption(_caption/*i18n("Edit Task")*/);

	// set focus to first widget
	if (ctTask->isSystemCrontab()) {
		leUser->setFocus();
	} else {
		leComment->setFocus();
	}

	// connect them up
	connect(command, SIGNAL(textChanged(const QString&)), SLOT(slotWizard()));

	connect(chkEnabled, SIGNAL(clicked()), SLOT(slotEnabledChanged()));
	connect(chkEnabled, SIGNAL(clicked()), SLOT(slotWizard()));

	connect(chkReboot, SIGNAL(clicked()), SLOT(slotRebootChanged()));
	connect(chkReboot, SIGNAL(clicked()), SLOT(slotWizard()));

	connect(cbEveryDay, SIGNAL(clicked()), SLOT(slotDailyChanged()));
	connect(cbEveryDay, SIGNAL(clicked()), SLOT(slotWizard()));

	connect(this, SIGNAL(okClicked()), SLOT(slotOK()));
	connect(this, SIGNAL(cancelClicked()), SLOT(slotCancel()));

	// key acceleration
	/*
	 key_accel = new KAccel(this);
	 key_accel->insert(KStandardShortcut::Open, this, SLOT(slotOK()));
	 key_accel->insert(KStandardShortcut::Close, this, SLOT(slotCancel()));
	 key_accel->insert(KStandardShortcut::Quit, this, SLOT(slotCancel()));
	 key_accel->readSettings();
	 */
	main->layout()->setSizeConstraint(QLayout::SetFixedSize);
	show();

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

KTTask::~KTTask() {

}

bool KTTask::isEveryDay() {
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		if (!ctTask->dayOfWeek.get(dw))
			return false;
	}

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		if (!ctTask->month.get(mo))
			return false;
	}

	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (!ctTask->dayOfMonth.get(dm))
			return false;
	}

	return true;
}

QGroupBox* KTTask::createDaysOfMonthGroup(QWidget* main) {

	QGroupBox* daysOfMonthGroup = new QGroupBox( i18n("Days of Month"), main);
	QVBoxLayout* vdays = new QVBoxLayout( daysOfMonthGroup );
	QHBoxLayout* hdays = 0;

	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if ( (dm % 7) == 1) {
			hdays = new QHBoxLayout();
			vdays->addLayout(hdays);
		}

		KTPushButton* day = new KTPushButton(daysOfMonthGroup);
		KAcceleratorManager::setNoAccel(day);
		day->setFixedSize(25, 25);
		day->setText(QString::number(dm));
		day->setCheckable(true);
		day->setChecked(ctTask->dayOfMonth.get(dm));
		pbDayOfMonth[dm] = day;

		connect(pbDayOfMonth[dm], SIGNAL(clicked()), SLOT(slotDayOfMonthChanged()));
		connect(pbDayOfMonth[dm], SIGNAL(clicked()), SLOT(slotWizard()));

		hdays->addWidget(day, Qt::AlignLeft);
	}
	hdays->addStretch(1);
	pbAllDaysOfMonth = new SetOrClearAllButton(daysOfMonthGroup, SetOrClearAllButton::SET_ALL);
	hdays->addWidget(pbAllDaysOfMonth, Qt::AlignLeft);

	connect(pbAllDaysOfMonth, SIGNAL(clicked()), SLOT(slotAllDaysOfMonth()));
	connect(pbAllDaysOfMonth, SIGNAL(clicked()), SLOT(slotWizard()));

	return daysOfMonthGroup;
}

QGroupBox* KTTask::createMonthsGroup(QWidget* main) {

	QGroupBox* monthsGroup = new QGroupBox( i18n("Months"), main);
	QGridLayout* monthsLayout = new QGridLayout(monthsGroup);

	int column = 0;
	int row = 0;

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		cbMonth[mo] = new KTPushButton(monthsGroup);
		cbMonth[mo]->setText(ctTask->month.getName(mo));
		cbMonth[mo]->setCheckable(true);
		cbMonth[mo]->setChecked(ctTask->month.get(mo));

		monthsLayout->addWidget(cbMonth[mo], row, column);

		connect(cbMonth[mo], SIGNAL(clicked()), SLOT(slotMonthChanged()));
		connect(cbMonth[mo], SIGNAL(clicked()), SLOT(slotWizard()));

		if (column == 1) {
			column = 0;
			row++;
		} else {
			column = 1;
		}
	}

	pbAllMonths = new SetOrClearAllButton(monthsGroup, SetOrClearAllButton::SET_ALL);
	monthsLayout->addWidget(pbAllMonths, row, 0, 1, 2);

	connect(pbAllMonths, SIGNAL(clicked()), SLOT(slotAllMonths()));
	connect(pbAllMonths, SIGNAL(clicked()), SLOT(slotWizard()));

	return monthsGroup;

}

QGroupBox* KTTask::createDaysOfWeekGroup(QWidget* main) {

	QGroupBox* daysOfWeekGroup = new QGroupBox( i18n("Days of Week"), main);
	QGridLayout* daysOfWeekLayout = new QGridLayout(daysOfWeekGroup);

	int column = 0;
	int row = 0;
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		cbDayOfWeek[dw] = new KTPushButton(daysOfWeekGroup);
		cbDayOfWeek[dw]->setText(ctTask->dayOfWeek.getName(dw));
		cbDayOfWeek[dw]->setCheckable(true);
		cbDayOfWeek[dw]->setChecked(ctTask->dayOfWeek.get(dw));
		daysOfWeekLayout->addWidget(cbDayOfWeek[dw], row, column);

		connect(cbDayOfWeek[dw], SIGNAL(clicked()), SLOT(slotDayOfWeekChanged()));
		connect(cbDayOfWeek[dw], SIGNAL(clicked()), SLOT(slotWizard()));

		if (column == 1) {
			column = 0;
			row++;
		} else {
			column = 1;
		}

	}

	pbAllDaysOfWeek = new SetOrClearAllButton(daysOfWeekGroup, SetOrClearAllButton::SET_ALL);
	daysOfWeekLayout->addWidget(pbAllDaysOfWeek);

	connect(pbAllDaysOfWeek, SIGNAL(clicked()), SLOT(slotAllDaysOfWeek()));
	connect(pbAllDaysOfWeek, SIGNAL(clicked()), SLOT(slotWizard()));

	return daysOfWeekGroup;
}

QGroupBox* KTTask::createMinutesGroup(QWidget* main) {
	// minutes
	QGroupBox* minutesGroup = new QGroupBox( i18n("Minutes"), main);
	QVBoxLayout *vmin = new QVBoxLayout( minutesGroup );

	for (int minuteIndex = 0; minuteIndex <= 55; minuteIndex+=5) {
		pbMinute[minuteIndex] = new KTPushButton(minutesGroup);
		KAcceleratorManager::setNoAccel(pbMinute[minuteIndex]);
		pbMinute[minuteIndex]->setText(QString::number(minuteIndex));
		pbMinute[minuteIndex]->setCheckable(true);
		pbMinute[minuteIndex]->setChecked(ctTask->minute.get(minuteIndex));
		pbMinute[minuteIndex]->setFixedSize(25, 25);

		connect(pbMinute[minuteIndex], SIGNAL(clicked()), SLOT(slotMinuteChanged()));
		connect(pbMinute[minuteIndex], SIGNAL(clicked()), SLOT(slotWizard()));
	}

	QHBoxLayout *hmin = new QHBoxLayout();
	vmin->addLayout(hmin);

	for (int minuteIndex = 0; minuteIndex <= 55; minuteIndex+=5) {
		if (minuteIndex == 30) {
			hmin = new QHBoxLayout();
			vmin->addLayout(hmin);
		}

		hmin->addWidget(pbMinute[minuteIndex]);
	}

	hmin = new QHBoxLayout();
	vmin->addLayout(hmin);

	pbAllMinutes = new SetOrClearAllButton(minutesGroup, SetOrClearAllButton::SET_ALL);
	hmin->addWidget(pbAllMinutes, Qt::AlignLeft);

	connect(pbAllMinutes, SIGNAL(clicked()), SLOT(slotAllMinutes()));
	connect(pbAllMinutes, SIGNAL(clicked()), SLOT(slotWizard()));

	return minutesGroup;
}

QGroupBox* KTTask::createHoursGroup(QWidget* main) {

	// hours
	QGroupBox* hoursGroup = new QGroupBox( i18n("Hours"), main);
	QVBoxLayout* v4 = new QVBoxLayout( hoursGroup );

	labAM = new QLabel( i18n("AM"), hoursGroup);
	labAM->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	v4->addWidget(labAM);

	for (int ho = 0; ho <= 23; ho++) {
		pbHour[ho] = new KTPushButton(hoursGroup);
		KAcceleratorManager::setNoAccel(pbHour[ho]);
		pbHour[ho]->setText(QString::number(ho));
		pbHour[ho]->setCheckable(true);
		pbHour[ho]->setChecked(ctTask->hour.get(ho));
		pbHour[ho]->setFixedSize(25, 25);
	}

	QHBoxLayout *hhours = new QHBoxLayout( );
	v4->addLayout(hhours);
	for (int ho1 = 0; ho1 <= 11; ho1++) {
		if (ho1 == 6) {
			hhours = new QHBoxLayout();
			v4->addLayout(hhours);
		}
		hhours->addWidget(pbHour[ho1]);

		connect(pbHour[ho1], SIGNAL(clicked()), SLOT(slotHourChanged()));
		connect(pbHour[ho1], SIGNAL(clicked()), SLOT(slotWizard()));
	}

	labPM = new QLabel( i18n("PM"), hoursGroup);
	labPM->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
	v4->addWidget(labPM);

	hhours = new QHBoxLayout();
	v4->addLayout(hhours);
	for (int ho1 = 12; ho1 <= 23; ho1++) {
		if (ho1 == 18) {
			hhours = new QHBoxLayout();
			v4->addLayout(hhours);
		}

		hhours->addWidget(pbHour[ho1]);

		connect(pbHour[ho1], SIGNAL(clicked()), SLOT(slotHourChanged()));
		connect(pbHour[ho1], SIGNAL(clicked()), SLOT(slotWizard()));
	}

	hhours = new QHBoxLayout( );
	v4->addLayout(hhours);

	pbAllHours = new SetOrClearAllButton(hoursGroup, SetOrClearAllButton::SET_ALL);
	hhours->addWidget(pbAllHours, Qt::AlignLeft);

	connect(pbAllHours, SIGNAL(clicked()), SLOT(slotAllHours()));
	connect(pbAllHours, SIGNAL(clicked()), SLOT(slotWizard()));

	return hoursGroup;
}

void KTTask::defineCommandIcon() {
	// try and get an icon for command
	QString qsCommand(command->url().path());

	// qsCommand broken down this way to split off qsCommand attributes
	int firstSpace(qsCommand.indexOf(" "));
	if (firstSpace > 0)
		qsCommand = qsCommand.left(firstSpace);
	int lastSlash(qsCommand.lastIndexOf("/"));
	if (lastSlash > 0)
		qsCommand = qsCommand.right(qsCommand.size() - lastSlash - 1);

	QPixmap qpIcon = SmallIcon(qsCommand);
	if (qpIcon.isNull())
		qpIcon = KTIcon::task(KTIcon::Small);

	commandIcon->setPixmap(qpIcon);
}

void KTTask::setupTitleWidget(const QString& comment, KTitleWidget::MessageType messageType) {
	titleWidget->setComment(comment, messageType);

	if (messageType == KTitleWidget::ErrorMessage)
		titleWidget->setPixmap(KIcon(KTIcon::error(KTIcon::Large)), KTitleWidget::ImageRight);
	else
		titleWidget->setPixmap(KIcon(KTIcon::task(KTIcon::Large)), KTitleWidget::ImageRight);

}

void KTTask::slotEnabledChanged() {
	bool enabled = chkEnabled->isChecked();
	leUser->setEnabled(enabled);
	leComment->setEnabled(enabled);
	command->setEnabled(enabled);
	chkReboot->setEnabled(enabled);

	// if chkReboot is already checked, allow setEnabled(false) but not setEnable(true) ...
	if (!chkReboot->isChecked() || !enabled) {
		cbEveryDay->setEnabled(enabled);
		bgHour->setEnabled(enabled);
		bgMinute->setEnabled(enabled);
	}

	// if cbEveryDay is already checked, allow setEnabled(false) but not setEnable(true) ...
	if ((!chkReboot->isChecked() && !cbEveryDay->isChecked()) || !enabled) {
		bgMonth->setEnabled(enabled);
		bgDayOfMonth->setEnabled(enabled);
		bgDayOfWeek->setEnabled(enabled);
	}
}

void KTTask::slotRebootChanged() {
	bool reboot = !chkReboot->isChecked();
	cbEveryDay->setEnabled(reboot);
	bgHour->setEnabled(reboot);
	bgMinute->setEnabled(reboot);

	// if cbEveryDay is already checked, bgMonth, bgDayOfMonth, bgDayOfWeek are already setEnable(flase)
	// so don't overide them ! ...
	if (!cbEveryDay->isChecked()) {
		bgMonth->setEnabled(reboot);
		bgDayOfMonth->setEnabled(reboot);
		bgDayOfWeek->setEnabled(reboot);
	}
}

void KTTask::slotDailyChanged() {
	if (cbEveryDay->isChecked()) {
		for (int mo = 1; mo <= 12; mo++)
			cbMonth[mo]->setChecked(true);
		for (int dm = 1; dm <= 31; dm++)
			pbDayOfMonth[dm]->setChecked(true);
		for (int dw = 1; dw <= 7; dw++)
			cbDayOfWeek[dw]->setChecked(true);
		bgMonth->setEnabled(false);
		bgDayOfMonth->setEnabled(false);
		bgDayOfWeek->setEnabled(false);
		pbAllMonths->setEnabled(false);
		pbAllDaysOfMonth->setEnabled(false);
		pbAllDaysOfWeek->setEnabled(false);
	} else {
		bgMonth->setEnabled(true);
		bgDayOfMonth->setEnabled(true);
		bgDayOfWeek->setEnabled(true);
		pbAllMonths->setEnabled(true);
		pbAllDaysOfMonth->setEnabled(true);
		pbAllDaysOfWeek->setEnabled(true);
	}

	slotMonthChanged();
	slotDayOfMonthChanged();
	slotDayOfWeekChanged();
}

void KTTask::slotOK() {
	// Make it friendly for just selecting days of the month or
	// days of the week.

	int monthDaysSelected(0);
	for (int dm = 1; dm <= 31; dm++) {
		if (pbDayOfMonth[dm]->isChecked())
			monthDaysSelected++;
	}

	int weekDaysSelected(0);
	for (int dw = 1; dw <= 7; dw++) {
		if (cbDayOfWeek[dw]->isChecked())
			weekDaysSelected++;
	}

	if ((monthDaysSelected == 0) && (weekDaysSelected > 0)) {
		for (int dm = 1; dm <= 31; dm++) {
			pbDayOfMonth[dm]->setChecked(true);
		}
	}

	if ((weekDaysSelected == 0) && (monthDaysSelected > 0)) {
		for (int dw = 1; dw <= 7; dw++) {
			cbDayOfWeek[dw]->setChecked(1);
		}
	}

	// save work in process
	if (!ctTask->user.isEmpty()) {
		ctTask->user = QFile::encodeName(leUser->text());
	} else {
		ctTask->user = "";
	}

	ctTask->comment = leComment->text();
	ctTask->command = command->url().path();
	ctTask->enabled = chkEnabled->isChecked();
	ctTask->reboot = chkReboot->isChecked();

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		ctTask->month.set(mo, cbMonth[mo]->isChecked());
	}

	for (int dm = 1; dm <= 31; dm++) {
		ctTask->dayOfMonth.set(dm, pbDayOfMonth[dm]->isChecked());
	}
	for (int dw = 1; dw <= 7; dw++) {
		ctTask->dayOfWeek.set(dw, cbDayOfWeek[dw]->isChecked());
	}
	for (int ho = 0; ho <= 23; ho++) {
		ctTask->hour.set(ho, pbHour[ho]->isChecked());
	}
	for (int mi = 0; mi <= 59; mi++) {
		ctTask->minute.set(mi, false);
	}
	for (int mi1 = 0; mi1 <= 55; mi1+=5) {
		ctTask->minute.set(mi1, pbMinute[mi1]->isChecked());
	}
	close();
}

void KTTask::slotWizard() {
	bool error = false;

	if (!chkEnabled->isChecked()) {
		setupTitleWidget(i18n("<i>This task is disabled.</i>"));
		KDialog::enableButtonOk(true);
		chkEnabled->setFocus();
		error = true;
	}

	if (chkReboot->isChecked() && !error) {
		setupTitleWidget(i18n("<i>This task will be run on system bootup.</i>"));
		KDialog::enableButtonOk(true);
		error = true;
	}

	if (command->url().path().isEmpty() && !error) {
		setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		command->setFocus();
		commandIcon->setPixmap(SmallIcon("image-missing"));
		error = true;
	}

	// make sure the file name is a good one if we have an
	// absolute path or if we are relying on $PATH
	QString pathstr, cmdstr, tmpstr;
	QString qs = command->url().path();

	if (qs.left(0) == "/" or qs.left(1) == "/") {
		// detected a '/' at the start - must be absolute path 
		tmpstr = qs.section(' ', 0, 0);
		pathstr = tmpstr.section('/', 0, -2);
		cmdstr = tmpstr.section('/', -1);
	}
	else {
		// relying on $PATH
		pathstr = QString(); // null string
		cmdstr = qs.section(' ', 0, 0);
	}

	logDebug() << "Looking for " << cmdstr << "in" << pathstr << endl;

	bool found = false;
	bool exec = false;
	if (KStandardDirs::findExe(cmdstr, pathstr, KStandardDirs::IgnoreExecBit) != "")
		found = true;
	if (KStandardDirs::findExe(cmdstr, pathstr) != "")
		exec = true;

	if (found && !exec && !error) {
		setupTitleWidget(i18n("<i>Please select an executable program...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		command->setFocus();
		commandIcon->setPixmap(SmallIcon("image-missing"));
		error = true;
	}

	if (!found && !error) {
		setupTitleWidget(i18n("<i>Please browse for a program to execute...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		command->setFocus();
		commandIcon->setPixmap(SmallIcon("image-missing"));
		error = true;
	}

	// the months
	bool valid(false);
	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		if (cbMonth[mo]->isChecked())
			valid = true;
	}
	
	if (!valid && !error) {
		setupTitleWidget(i18n("<i>Please select from the 'Months' section...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		cbMonth[1]->setFocus();
		error = true;
	}

	// the days
	valid = false;
	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (pbDayOfMonth[dm]->isChecked())
			valid = true;
	}
	for (int dw = CTDayOfWeek::MINIMUM; dw <= CTDayOfWeek::MAXIMUM; dw++) {
		if (cbDayOfWeek[dw]->isChecked())
			valid = true;
	}

	if (!valid && !error) {
		setupTitleWidget(i18n("<i>Please select from either the 'Days of Month' or the 'Days of Week' section...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		pbDayOfMonth[1]->setFocus();
		error = true;
	}

	// the hours
	valid = false;
	for (int ho = 0; ho <= 23; ho++) {
		if (pbHour[ho]->isChecked())
			valid = true;
	}

	if (!valid && !error) {
		setupTitleWidget(i18n("<i>Please select from the 'Hours' section...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		pbHour[0]->setFocus();
		error = true;
	}

	// the mins
	valid = false;
	for (int mi1 = 0; mi1 <= 55; mi1+=5) {
		if (pbMinute[mi1]->isChecked())
			valid = true;
	}

	if (!valid && !error) {
		setupTitleWidget(i18n("<i>Please select from the 'Minutes' section...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		pbMinute[0]->setFocus();
		error = true;
	}

	if (!error) {
		defineCommandIcon();
		setupTitleWidget(i18n("<i>This task will be executed at the specified intervals.</i>"));
		KDialog::enableButtonOk(true);
	}
}

void KTTask::slotCancel() {
	close();
}

void KTTask::slotAllMonths() {
	bool checked = false;
	if (pbAllMonths->isSetAll()) {
		checked = true;
	}

	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		cbMonth[mo]->setChecked(checked);
	}

	slotMonthChanged();
}

void KTTask::slotMonthChanged() {
	bool allCleared = true;
	for (int mo = CTMonth::MINIMUM; mo <= CTMonth::MAXIMUM; mo++) {
		if (cbMonth[mo]->isChecked()) {
			allCleared = false;
			break;
		}
	}

	if (allCleared) {
		pbAllMonths->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		pbAllMonths->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void KTTask::slotAllDaysOfMonth() {
	bool checked = false;
	if (pbAllDaysOfMonth->isSetAll()) {
		checked = true;
	}

	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		pbDayOfMonth[dm]->setChecked(checked);
	}

	slotDayOfMonthChanged();
}

void KTTask::slotDayOfMonthChanged() {
	bool allCleared = true;
	for (int dm = CTDayOfMonth::MINIMUM; dm <= CTDayOfMonth::MAXIMUM; dm++) {
		if (pbDayOfMonth[dm]->isChecked()) {
			allCleared = false;
			break;
		}
	}

	if (allCleared) {
		pbAllDaysOfMonth->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		pbAllDaysOfMonth->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void KTTask::slotAllDaysOfWeek() {
	if (pbAllDaysOfWeek->isSetAll()) {
		for (int dw = 1; dw <= 7; dw++) {
			cbDayOfWeek[dw]->setChecked(true);
		}
	} else {
		for (int dw = 1; dw <= 7; dw++) {
			cbDayOfWeek[dw]->setChecked(false);
		}
	}
	slotDayOfWeekChanged();
}

void KTTask::slotDayOfWeekChanged() {
	bool allChecked = true;
	bool allCleared = true;
	for (int dw = 1; dw <= 7; dw++) {
		if (cbDayOfWeek[dw]->isChecked()) {
			allCleared = false;
		} else {
			allChecked = false;
		}
	}
	if (allCleared) {
		pbAllDaysOfWeek->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		pbAllDaysOfWeek->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void KTTask::slotAllHours() {
	if (pbAllHours->isSetAll()) {
		for (int ho = 0; ho <= 23; ho++) {
			pbHour[ho]->setChecked(true);
		}
	} else {
		for (int ho = 0; ho <= 23; ho++) {
			pbHour[ho]->setChecked(false);
		}
	}
	slotHourChanged();
}

void KTTask::slotHourChanged() {
	bool allChecked = true;
	bool allCleared = true;
	for (int ho = 0; ho <= 23; ho++) {
		if (pbHour[ho]->isChecked()) {
			allCleared = false;
		} else {
			allChecked = false;
		}
	}
	if (allCleared) {
		pbAllHours->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		pbAllHours->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

void KTTask::slotAllMinutes() {
	if (pbAllMinutes->isSetAll()) {
		for (int mi = 0; mi <= 55; mi+=5) {
			pbMinute[mi]->setChecked(true);
		}
	} else {
		for (int mi = 0; mi <= 55; mi+=5) {
			pbMinute[mi]->setChecked(false);
		}
	}
	slotMinuteChanged();
}

void KTTask::slotMinuteChanged() {
	bool allChecked = true;
	bool allCleared = true;
	for (int mi = 0; mi <= 55; mi+=5) {
		if (pbMinute[mi]->isChecked()) {
			allCleared = false;
		} else {
			allChecked = false;
		}
	}
	if (allCleared) {
		pbAllMinutes->setStatus(SetOrClearAllButton::SET_ALL);
	} else {
		pbAllMinutes->setStatus(SetOrClearAllButton::CLEAR_ALL);
	}
}

#include "kttask.moc"
