/***************************************************************************
 *   KT task editor window header                                          *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef TASK_EDITOR_DIALOG_H
#define TASK_EDITOR_DIALOG_H

#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QDialog>
#include <QStringList>
#include <QTextEdit>

#include <KTitleWidget>

class QLabel;
class QCheckBox;
class QGridLayout;
class QHBoxLayout;

class KUrlRequester;

class CTTask;

class CrontabWidget;

class SetOrClearAllButton : public QPushButton {
	
public:
	
	enum Status {
		SET_ALL,
		CLEAR_ALL
	};
	
	SetOrClearAllButton(QWidget * parent, SetOrClearAllButton::Status status);
	
	void setStatus(SetOrClearAllButton::Status status);
	
	bool isSetAll();
	
	bool isClearAll();
	
private:
	
	SetOrClearAllButton::Status currentStatus;
	
};

class NumberPushButton : public QPushButton {
public:
	
	explicit NumberPushButton(QWidget * parent);
	NumberPushButton(bool digitMode, QWidget * parent);
	
	void updatePalette();
	
	bool event(QEvent *e) Q_DECL_OVERRIDE;
	
	void paintEvent(QPaintEvent*) Q_DECL_OVERRIDE;

	bool isDirty;
	QPalette palSelected;
	QPalette palNormal;
};

/**
 * Task editor window.
 */
class TaskEditorDialog : public QDialog {
Q_OBJECT

public:

	/**
	 * Initialize from CTTask.
	 */
	explicit TaskEditorDialog(CTTask* ctTask, const QString& caption, CrontabWidget* crontabWidget);

	/**
	 * Destroy.
	 */
	~TaskEditorDialog();

	
	
private Q_SLOTS:


	/**
	 * Control the task title bar.
	 */
	void setupTitleWidget(const QString& comment=QLatin1String(""), KTitleWidget::MessageType = KTitleWidget::PlainMessage);

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
	NumberPushButton* createHourButton(QGroupBox* hoursGroup, int hour);
	QGroupBox* createHoursGroup(QWidget* mainWidget);
	
	NumberPushButton* createMinuteButton(int minuteIndex);
	void createMinutesGroup(QWidget* mainWidget);
	
	/**
	 * Returns true if there is no checked minute in the hidden minuteButton
	 */
	bool canReduceMinutesGroup();
	void emptyMinutesGroup();
	void reduceMinutesGroup();
	void increaseMinutesGroup();

	QGroupBox* createMonthsGroup(QWidget* mainWidget);
	
	QGroupBox* createDaysOfMonthGroup(QWidget* mainWidget);
	QGroupBox* createDaysOfWeekGroup(QWidget* mainWidget);
	
	bool checkCommand();
	
	void defineCommandIcon();
	
	bool isEveryDay();
	
	/**
	 * Task.
	 */
	CTTask* ctTask;
	
	CrontabWidget* crontabWidget;

	// Widgets.

	KTitleWidget* titleWidget;
	QPushButton* okButton;

	QComboBox* userCombo;

	QTextEdit* leComment;

	QLabel* commandIcon;
	QPixmap missingCommandPixmap;
	KUrlRequester* command;

	QCheckBox* chkEnabled;
	QCheckBox* chkReboot;
	QCheckBox* cbEveryDay;

	QGroupBox* bgMonth;
	NumberPushButton* monthButtons[13]; // The index 0 is not used
	SetOrClearAllButton* allMonths;

	QGroupBox* bgDayOfMonth;
	NumberPushButton* dayOfMonthButtons[32]; // The index 0 is not used
	SetOrClearAllButton* allDaysOfMonth;

	QGroupBox* bgDayOfWeek;
	NumberPushButton* dayOfWeekButtons[8]; // The index 0 is not used
	SetOrClearAllButton* allDaysOfWeek;

	QGroupBox* hoursGroup;
	QLabel* morningLabel;
	QLabel* afternoonLabel;
	QPushButton* hourButtons[24];
	SetOrClearAllButton* allHours;

	QGroupBox* minutesGroup;
	QGridLayout* minutesLayout;
	QPushButton* minuteButtons[60];
	
	QHBoxLayout* minutesPreselectionLayout;
	QComboBox* minutesPreselection;
	
	static const int minuteTotal = 59; // or 55 or 59
	
	static const int minutePerColumn = 12; // or 30 or 12
	
	static const int reducedMinuteStep = 5;
	
	QStringList specialValidCommands;

};

#endif // TASK_EDITOR_DIALOG_H
