/***************************************************************************
 *   KT environment variable editor window implementation                  *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include "variableEditorDialog.h"

#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QLineEdit>

#include <klineedit.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktitlewidget.h>

#include "ctvariable.h"
#include "cthost.h"
#include "ctcron.h"

#include "crontabWidget.h"

#include "kcronIcons.h"
#include "kcronHelper.h"
#include <KDebug>
VariableEditorDialog::VariableEditorDialog(CTVariable* _ctVariable, const QString &_caption, CrontabWidget* _crontabWidget) :
    KDialog(_crontabWidget) {
	ctVariable = _ctVariable;
	crontabWidget = _crontabWidget;

	setModal(true);
	setCaption(_caption);
	setButtons(Ok|Cancel);
	setDefaultButton(Ok);

	QWidget* page = new QWidget;
	QGridLayout* layout = new QGridLayout;
	page->setLayout(layout);

	layout->setMargin(0);
	layout->setColumnMinimumWidth(1, 270);
	layout->setRowStretch(3, 1);
	layout->setColumnStretch(1, 1);

	setMainWidget(page);

	setWindowIcon(KCronIcons::application(KCronIcons::Small));

	int layoutPosition = 0;

	// top title widget
	titleWidget = new KTitleWidget(this);
	titleWidget->setText(i18n("Add or modify a variable"));
	layout->addWidget(titleWidget, layoutPosition, 0, 1, 2);

        // variable
	QLabel* labVariable = new QLabel(i18nc("The environmental variable name ie HOME, MAILTO etc", "&Variable:"), this);
	layout->addWidget(labVariable, ++layoutPosition, 0, Qt::AlignLeft);

	cmbVariable = new QComboBox(this);
	cmbVariable->setEditable(true);
	layout->addWidget(cmbVariable, layoutPosition, 1);

	cmbVariable->addItem(QLatin1String( "HOME" ));
	cmbVariable->addItem(QLatin1String( "MAILTO" ));
	cmbVariable->addItem(QLatin1String( "PATH" ));
	cmbVariable->addItem(QLatin1String( "SHELL" ));
	cmbVariable->addItem(QLatin1String( "LD_CONFIG_PATH" ));

	labVariable->setBuddy(cmbVariable);

	// details
	QLabel* labDetails = new QLabel(QLatin1String( "" ), this);
	layout->addWidget(labDetails, ++layoutPosition, 0, Qt::AlignLeft);

	QHBoxLayout* detailsLayout = new QHBoxLayout;
	detailsIcon = new QLabel(this);
	detailsLayout->addWidget(detailsIcon);

	details = new QLabel(this);
	detailsLayout->addWidget(details);

	layout->addLayout(detailsLayout, layoutPosition, 1, Qt::AlignLeft);

	// value
	QLabel* labValue = new QLabel(i18n("Va&lue:"), this);
	layout->addWidget(labValue, ++layoutPosition, 0, Qt::AlignLeft);

	leValue = new QLineEdit(this);
	layout->addWidget(leValue, layoutPosition, 1);
	leValue->setMaxLength(255);
	labValue->setBuddy(leValue);

	// User Combo
	QLabel* userLabel = new QLabel( i18n("&Run as:"), this);
	layout->addWidget(userLabel, ++layoutPosition, 0);

	userCombo = new QComboBox(this);

	userLabel->setBuddy(userCombo);
	layout->addWidget(userCombo, layoutPosition, 1);

	if (crontabWidget->variablesWidget()->needUserColumn()) {
		KCronHelper::initUserCombo(userCombo, crontabWidget, ctVariable->userLogin);

	}
	else {
		userLabel->hide();
		userCombo->hide();
	}

	// comment
	QLabel* labComment = new QLabel(i18n("Co&mment:"), this);
	layout->addWidget(labComment, ++layoutPosition, 0, Qt::AlignLeft);

	teComment = KCronHelper::createCommentEdit(this);
	layout->addWidget(teComment, layoutPosition, 1);
	labComment->setBuddy(teComment);

	// enabled
	chkEnabled = new QCheckBox(i18n("&Enable this variable"), this);
	layout->addWidget(chkEnabled, ++layoutPosition, 0, 1, 2);

	// set starting field values
	cmbVariable->setEditText(ctVariable->variable);
	leValue->setText(ctVariable->value);
	teComment->setText(ctVariable->comment);
	chkEnabled->setChecked(ctVariable->enabled);
	cmbVariable->setFocus();

	slotEnabled();
	slotWizard();
	show();

	// connect them up
	connect(cmbVariable, SIGNAL(editTextChanged(QString)), SLOT(slotWizard()));
	connect(leValue, SIGNAL(textEdited(QString)), SLOT(slotWizard()));
	connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
	connect(chkEnabled, SIGNAL(clicked()), SLOT(slotEnabled()));

}

VariableEditorDialog::~VariableEditorDialog() {
}

void VariableEditorDialog::setupTitleWidget(const QString& comment, KTitleWidget::MessageType messageType) {
	//krazy:exclude=doublequote_chars
	if (comment.isEmpty()) {
		titleWidget->setComment(i18n("<i>This variable will be used by scheduled tasks.</i>"));
		titleWidget->setPixmap(KCronIcons::variable(KCronIcons::Large), KTitleWidget::ImageRight);
	}
	else {
		titleWidget->setComment(comment, messageType);
		if (messageType == KTitleWidget::ErrorMessage)
			titleWidget->setPixmap(KIcon(KCronIcons::error(KCronIcons::Large)), KTitleWidget::ImageRight);
		else
			titleWidget->setPixmap(KIcon(KCronIcons::information(KCronIcons::Large)), KTitleWidget::ImageRight);
	}
}

void VariableEditorDialog::slotEnabled() {
	bool enabled = chkEnabled->isChecked();
	cmbVariable->setEnabled(enabled);
	leValue->setEnabled(enabled);
	teComment->setEnabled(enabled);
	userCombo->setEnabled(enabled);
}

void VariableEditorDialog::slotOk() {
	ctVariable->variable = cmbVariable->currentText();
	ctVariable->value = leValue->text();
	ctVariable->comment = teComment->toPlainText();
	ctVariable->enabled = chkEnabled->isChecked();

	// save work in process
	if (crontabWidget->variablesWidget()->needUserColumn()) {
		ctVariable->userLogin = userCombo->currentText();
	}

	close();
}

void VariableEditorDialog::slotWizard() {
	CTVariable tempVariable(*ctVariable);
	tempVariable.variable = cmbVariable->currentText();

	detailsIcon->setPixmap(tempVariable.variableIcon());
	details->setText(tempVariable.information());

	bool error = false;

	if (!chkEnabled->isChecked()) {
		setupTitleWidget(i18n("<i>This variable is disabled.</i>"));
		chkEnabled->setFocus();
		KDialog::enableButtonOk(true);
		error = true;
	}

	if (cmbVariable->currentText().isEmpty() && !error) {
		setupTitleWidget(i18n("<i>Please enter the variable name...</i>"), KTitleWidget::ErrorMessage);
		cmbVariable->setFocus();
		KDialog::enableButtonOk(false);
		error = true;
	}

	if (leValue->text().isEmpty() && !error) {
		setupTitleWidget(i18n("<i>Please enter the variable value ...</i>"), KTitleWidget::ErrorMessage);
		KDialog::enableButtonOk(false);
		error = true;
	}

	if (!error) {
		setupTitleWidget();
		KDialog::enableButtonOk(true);
	}
}

#include "variableEditorDialog.moc"
