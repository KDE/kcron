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

#ifndef KTVARIABLE_H
#define KTVARIABLE_H

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTextEdit>

#include <klineedit.h>
#include <kdialog.h>
#include <ktitlewidget.h>

class CTVariable;

/**
 * Environment variable editor window.
 */
class KTVariable : public KDialog {
Q_OBJECT

public:

	/**
	 * Initialize from CTVariable.
	 */
	explicit KTVariable(CTVariable* _ctvar = 0, const QString &_caption="");

	/**
	 * Destroy.
	 */
	~KTVariable();

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
	CTVariable* ctvar;

	// Widgets.

	KTitleWidget* titleWidget;

	QComboBox* cmbVariable;

	QLabel* detailsIcon;
	QLabel* details;
	
	QLineEdit* leValue;

	KLineEdit* teComment;

	QCheckBox* chkEnabled;
};

#endif // KTVARIABLE_H
