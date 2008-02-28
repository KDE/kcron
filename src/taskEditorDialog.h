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
#include <QStringList>

#include <kdialog.h>
#include <kicon.h>
#include <ktitlewidget.h>

class QLabel;
class QLineEdit;
class QCheckBox;

class KUrlRequester;

class CTHost;
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

class KTPushButton : public QPushButton {
public:
	
	KTPushButton(QWidget * parent);
	
	void updatePalette();
	
	bool event(QEvent *e);
	
	void paintEvent(QPaintEvent*);

	bool isDirty;
	QPalette palSelected;
	QPalette palNormal;
};

/**
 * Task editor window.
 */
class TaskEditorDialog : public KDialog {
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

	
	
private slots:


	/**
	 * Control the task title bar.
	 */
	void setupTitleWidget(const QString& comment="", KTitleWidget::MessageType = KTitleWidget::PlainMessage);

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
	 * Cancel and close.
	 */
	void slotCancel();

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

	/**
	 * Set or clear all minute checkboxes
	 */
	void slotAllMinutes();

	/**
	 * A minute checkbox has changed
	 */
	void slotMinuteChanged();

private:
	QGroupBox* createHoursGroup(QWidget* mainWidget);
	QGroupBox* createMinutesGroup(QWidget* mainWidget);
	
	QGroupBox* createMonthsGroup(QWidget* mainWidget);
	
	QGroupBox* createDaysOfMonthGroup(QWidget* mainWidget);
	QGroupBox* createDaysOfWeekGroup(QWidget* mainWidget);
	
	void defineCommandIcon();

	
	bool isEveryDay();
	
	/**
	 * Task.
	 */
	CTTask* ctTask;
	
	CrontabWidget* crontabWidget;

	// Widgets.

	KTitleWidget* titleWidget;

	QComboBox* userCombo;

	QLineEdit* leComment;

	QLabel* commandIcon;
	KUrlRequester* command;

	QCheckBox* chkEnabled;
	QCheckBox* chkReboot;
	QCheckBox* cbEveryDay;

	QGroupBox* bgMonth;
	KTPushButton* cbMonth[13];
	SetOrClearAllButton* pbAllMonths;

	QGroupBox* bgDayOfMonth;
	KTPushButton* pbDayOfMonth[32];
	SetOrClearAllButton* pbAllDaysOfMonth;

	QGroupBox* bgDayOfWeek;
	KTPushButton* cbDayOfWeek[8];
	SetOrClearAllButton* pbAllDaysOfWeek;

	QGroupBox* bgHour;
	QLabel* labAM;
	QLabel* labPM;
	QPushButton* pbHour[25];
	SetOrClearAllButton* pbAllHours;

	QGroupBox* bgMinute;
	QPushButton* pbMinute[61];
	SetOrClearAllButton* pbAllMinutes;
	
	QStringList specialValidCommands;

};

#endif // TASK_EDITOR_DIALOG_H
