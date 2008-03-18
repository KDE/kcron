/***************************************************************************
 *   KT environment variable editor window header                          *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#ifndef VARIABLE_EDITOR_DIALOG_H
#define VARIABLE_EDITOR_DIALOG_H

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>

#include <klineedit.h>
#include <kdialog.h>
#include <ktitlewidget.h>

class CTVariable;
class CrontabWidget;

/**
 * Environment variable editor window.
 */
class VariableEditorDialog : public KDialog {
Q_OBJECT

public:

	/**
	 * Initialize from CTVariable.
	 */
	explicit VariableEditorDialog(CTVariable* _ctVariable, const QString &_caption, CrontabWidget* crontabWidget);

	/**
	 * Destroy.
	 */
	~VariableEditorDialog();

private slots:

	/**
	 * Setup title widget
	 */
	void setupTitleWidget(const QString& comment = "", KTitleWidget::MessageType = KTitleWidget::PlainMessage);

	/**
	 * Enable / disable variable
	 */
	void slotEnabled();

	/**
	 * Apply changes and close.
	 */
	void slotOk();

	/**
	 * Run the wizard.
	 */
	void slotWizard();

private:

	/**
	 * Environment variable.
	 */
	CTVariable* ctVariable;

	CrontabWidget* crontabWidget;
	
	// Widgets.
	KTitleWidget* titleWidget;

	QComboBox* cmbVariable;

	QLabel* detailsIcon;
	QLabel* details;
	
	QLineEdit* leValue;

	QTextEdit* teComment;

	QCheckBox* chkEnabled;
	
	QComboBox* userCombo;
};

#endif // VARIABLE_EDITOR_DIALOG_H
