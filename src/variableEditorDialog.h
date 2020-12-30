/*
    KT environment variable editor window header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef VARIABLE_EDITOR_DIALOG_H
#define VARIABLE_EDITOR_DIALOG_H

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDialog>
#include <QTextEdit>

#include <KTitleWidget>

class CTVariable;
class CrontabWidget;

/**
 * Environment variable editor window.
 */
class VariableEditorDialog : public QDialog {
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

private Q_SLOTS:

	/**
	 * Setup title widget
	 */
	void setupTitleWidget(const QString& comment = QLatin1String(""), KTitleWidget::MessageType = KTitleWidget::PlainMessage);

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
	QPushButton* okButton;

	QComboBox* cmbVariable;

	QLabel* detailsIcon;
	QLabel* details;
	
	QLineEdit* leValue;

	QTextEdit* teComment;

	QCheckBox* chkEnabled;
	
	QComboBox* userCombo;
};

#endif // VARIABLE_EDITOR_DIALOG_H
