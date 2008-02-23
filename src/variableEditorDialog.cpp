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
#include "kticon.h"

VariableEditorDialog::VariableEditorDialog(CTVariable* _ctvar, const QString &_caption) :
	KDialog(), ctvar(_ctvar) {

	setModal(true);
	setCaption(_caption);
	setButtons(Ok|Cancel);
	setDefaultButton(Ok);

	QWidget* page = new QWidget(this);
	QGridLayout* layout = new QGridLayout();
	page->setLayout(layout);

	layout->setMargin(0);
	layout->setSpacing(spacingHint() );
	layout->setColumnMinimumWidth(1, 270);
	layout->setRowStretch(3, 1);
	layout->setColumnStretch(1, 1);

	setMainWidget(page);

	setWindowIcon(KTIcon::application(KTIcon::Small));

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

	cmbVariable->addItem("HOME");
	cmbVariable->addItem("MAILTO");
	cmbVariable->addItem("PATH");
	cmbVariable->addItem("SHELL");

	labVariable->setBuddy(cmbVariable);
	
	// details
	QLabel* labDetails = new QLabel("", this);
	layout->addWidget(labDetails, ++layoutPosition, 0, Qt::AlignLeft);

	QHBoxLayout* detailsLayout = new QHBoxLayout(this);
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

	// comment
	QLabel* labComment = new QLabel(i18n("Co&mment:"), this);
	layout->addWidget(labComment, ++layoutPosition, 0, Qt::AlignLeft);

	teComment = new QLineEdit(this);
	layout->addWidget(teComment, layoutPosition, 1);
	labComment->setBuddy(teComment);

	// enabled
	chkEnabled = new QCheckBox(i18n("&Enable this environment variable"), this);
	layout->addWidget(chkEnabled, ++layoutPosition, 0, 1, 2);

	// set starting field values
	cmbVariable->setEditText(ctvar->variable);
	leValue->setText(ctvar->value);
	teComment->setText(ctvar->comment);
	chkEnabled->setChecked(ctvar->enabled);
	cmbVariable->setFocus();

	slotEnabled();
	slotWizard();
	show();

	// connect them up
	connect(cmbVariable, SIGNAL(editTextChanged(const QString&)), SLOT(slotWizard()));
	connect(leValue, SIGNAL(textEdited(const QString&)), SLOT(slotWizard()));
	connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
	connect(chkEnabled, SIGNAL(clicked()), SLOT(slotEnabled()));
}

VariableEditorDialog::~VariableEditorDialog() {
}

void VariableEditorDialog::setupTitleWidget(const QString& comment, KTitleWidget::MessageType messageType) {
	//krazy:exclude=doublequote_chars
	if (comment.isEmpty()) {
		titleWidget->setComment(i18n("<i>This variable will be used by scheduled tasks.</i>"));
		titleWidget->setPixmap(KTIcon::variable(KTIcon::Large), KTitleWidget::ImageRight);
	}
	else {
		titleWidget->setComment(comment, messageType);
		if (messageType == KTitleWidget::ErrorMessage)
			titleWidget->setPixmap(KIcon(KTIcon::error(KTIcon::Large)), KTitleWidget::ImageRight);
		else
			titleWidget->setPixmap(KIcon(KTIcon::information(KTIcon::Large)), KTitleWidget::ImageRight);
	}
}

void VariableEditorDialog::slotEnabled() {
	bool enabled = chkEnabled->isChecked();
	cmbVariable->setEnabled(enabled);
	leValue->setEnabled(enabled);
	teComment->setEnabled(enabled);
}

void VariableEditorDialog::slotOk() {
	ctvar->variable = cmbVariable->currentText();
	ctvar->value = leValue->text();
	ctvar->comment = teComment->text().replace('\n',' ').replace('\r',' ');
	ctvar->enabled = chkEnabled->isChecked();
	close();
}

void VariableEditorDialog::slotWizard() {
	QString variable = cmbVariable->currentText();
	if (variable == "HOME") {
		detailsIcon->setPixmap(KTIcon::home(KTIcon::Small));
		details->setText(i18n("Override default home folder."));
	}
	else if (variable == "MAILTO") {
		detailsIcon->setPixmap(KTIcon::mail(KTIcon::Small));
		details->setText(i18n("Email output to specified account."));
	}
	else if (variable == "SHELL") {
		detailsIcon->setPixmap(KTIcon::shell(KTIcon::Small));
		details->setText(i18n("Override default shell."));
	}
	else if (variable == "PATH") {
		detailsIcon->setPixmap(KTIcon::path(KTIcon::Small));
		details->setText(i18n("Folders to search for program files."));
	}
	else {
		detailsIcon->setPixmap(KTIcon::variable(KTIcon::Small));
		details->setText(i18n("Local Variable"));
	}

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
