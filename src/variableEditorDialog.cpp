/*
    KT environment variable editor window implementation
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "variableEditorDialog.h"
#include <QGridLayout>
#include <QPushButton>

#include <KLocalizedString>
#include <QDialogButtonBox>
#include <QIcon>

#include "ctcron.h"
#include "cthost.h"
#include "ctvariable.h"

#include "crontabWidget.h"

#include "kcronHelper.h"

VariableEditorDialog::VariableEditorDialog(CTVariable *_ctVariable, const QString &_caption, CrontabWidget *_crontabWidget)
    : QDialog(_crontabWidget)
{
    mCtVariable = _ctVariable;
    mCrontabWidget = _crontabWidget;

    setModal(true);
    setWindowTitle(_caption);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);

    QGridLayout *layout = new QGridLayout;
    setLayout(layout);

    layout->setColumnMinimumWidth(1, 270);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(1, 1);

    setWindowIcon(QIcon::fromTheme(QStringLiteral("kcron")));

    int layoutPosition = 0;

    // top title widget
    mTitleWidget = new KTitleWidget(this);
    mTitleWidget->setText(i18n("Add or modify a variable"));
    layout->addWidget(mTitleWidget, layoutPosition, 0, 1, 2);

    // variable
    QLabel *labVariable = new QLabel(i18nc("The environmental variable name ie HOME, MAILTO etc", "&Variable:"), this);
    layout->addWidget(labVariable, ++layoutPosition, 0, Qt::AlignLeft);

    mCmbVariable = new QComboBox(this);
    mCmbVariable->setEditable(true);
    layout->addWidget(mCmbVariable, layoutPosition, 1);

    mCmbVariable->addItem(QStringLiteral("HOME"));
    mCmbVariable->addItem(QStringLiteral("MAILTO"));
    mCmbVariable->addItem(QStringLiteral("PATH"));
    mCmbVariable->addItem(QStringLiteral("SHELL"));
    mCmbVariable->addItem(QStringLiteral("LD_CONFIG_PATH"));

    labVariable->setBuddy(mCmbVariable);

    // details
    QLabel *labDetails = new QLabel(QLatin1String(""), this);
    layout->addWidget(labDetails, ++layoutPosition, 0, Qt::AlignLeft);

    QHBoxLayout *detailsLayout = new QHBoxLayout;
    mDetailsIcon = new QLabel(this);
    detailsLayout->addWidget(mDetailsIcon);

    mDetails = new QLabel(this);
    detailsLayout->addWidget(mDetails);

    layout->addLayout(detailsLayout, layoutPosition, 1, Qt::AlignLeft);

    // value
    QLabel *labValue = new QLabel(i18n("Va&lue:"), this);
    layout->addWidget(labValue, ++layoutPosition, 0, Qt::AlignLeft);

    mLeValue = new QLineEdit(this);
    layout->addWidget(mLeValue, layoutPosition, 1);
    mLeValue->setMaxLength(255);
    labValue->setBuddy(mLeValue);

    // User Combo
    QLabel *userLabel = new QLabel(i18n("&Run as:"), this);
    layout->addWidget(userLabel, ++layoutPosition, 0);

    mUserCombo = new QComboBox(this);

    userLabel->setBuddy(mUserCombo);
    layout->addWidget(mUserCombo, layoutPosition, 1);

    if (mCrontabWidget->variablesWidget()->needUserColumn()) {
        KCronHelper::initUserCombo(mUserCombo, mCrontabWidget, mCtVariable->userLogin);
    } else {
        userLabel->hide();
        mUserCombo->hide();
    }

    // comment
    QLabel *labComment = new QLabel(i18n("Co&mment:"), this);
    layout->addWidget(labComment, ++layoutPosition, 0, Qt::AlignLeft);

    mTeComment = KCronHelper::createCommentEdit(this);
    layout->addWidget(mTeComment, layoutPosition, 1);
    labComment->setBuddy(mTeComment);

    // enabled
    mChkEnabled = new QCheckBox(i18n("&Enable this variable"), this);
    layout->addWidget(mChkEnabled, ++layoutPosition, 0, 1, 2);

    layout->addWidget(buttonBox, ++layoutPosition, 0, 1, 2);

    // set starting field values
    mCmbVariable->setEditText(mCtVariable->variable);
    mLeValue->setText(mCtVariable->value);
    mTeComment->setText(mCtVariable->comment);
    mChkEnabled->setChecked(mCtVariable->enabled);
    mCmbVariable->setFocus();

    slotEnabled();
    slotWizard();
    show();

    // connect them up
    connect(mCmbVariable, &QComboBox::editTextChanged, this, &VariableEditorDialog::slotWizard);
    connect(mLeValue, &QLineEdit::textEdited, this, &VariableEditorDialog::slotWizard);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &VariableEditorDialog::slotOk);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(mChkEnabled, &QCheckBox::clicked, this, &VariableEditorDialog::slotEnabled);
}

VariableEditorDialog::~VariableEditorDialog()
{
}

void VariableEditorDialog::setupTitleWidget(const QString &comment, KTitleWidget::MessageType messageType)
{
    // krazy:exclude=doublequote_chars
    if (comment.isEmpty()) {
        mTitleWidget->setComment(i18n("<i>This variable will be used by scheduled tasks.</i>"));
        mTitleWidget->setIcon(QIcon::fromTheme(QStringLiteral("text-plain")), KTitleWidget::ImageRight);
    } else {
        mTitleWidget->setComment(comment, messageType);
        if (messageType == KTitleWidget::ErrorMessage) {
            mTitleWidget->setIcon(QIcon::fromTheme(QStringLiteral("dialog-error")), KTitleWidget::ImageRight);
        } else {
            mTitleWidget->setIcon(QIcon::fromTheme(QStringLiteral("dialog-information")), KTitleWidget::ImageRight);
        }
    }
}

void VariableEditorDialog::slotEnabled()
{
    bool enabled = mChkEnabled->isChecked();
    mCmbVariable->setEnabled(enabled);
    mLeValue->setEnabled(enabled);
    mTeComment->setEnabled(enabled);
    mUserCombo->setEnabled(enabled);
}

void VariableEditorDialog::slotOk()
{
    mCtVariable->variable = mCmbVariable->currentText();
    mCtVariable->value = mLeValue->text();
    mCtVariable->comment = mTeComment->toPlainText();
    mCtVariable->enabled = mChkEnabled->isChecked();

    // save work in process
    if (mCrontabWidget->variablesWidget()->needUserColumn()) {
        mCtVariable->userLogin = mUserCombo->currentText();
    }

    accept();
}

void VariableEditorDialog::slotWizard()
{
    CTVariable tempVariable(*mCtVariable);
    tempVariable.variable = mCmbVariable->currentText();

    mDetailsIcon->setPixmap(tempVariable.variableIcon().pixmap(style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, this)));
    mDetails->setText(tempVariable.information());

    bool error = false;

    if (!mChkEnabled->isChecked()) {
        setupTitleWidget(i18n("<i>This variable is disabled.</i>"));
        mChkEnabled->setFocus();
        mOkButton->setEnabled(true);
        error = true;
    }

    if (mCmbVariable->currentText().isEmpty() && !error) {
        setupTitleWidget(i18n("<i>Please enter the variable name...</i>"), KTitleWidget::ErrorMessage);
        mCmbVariable->setFocus();
        mOkButton->setEnabled(false);
        error = true;
    }

    if (mLeValue->text().isEmpty() && !error) {
        setupTitleWidget(i18n("<i>Please enter the variable value ...</i>"), KTitleWidget::ErrorMessage);
        mOkButton->setEnabled(false);
        error = true;
    }

    if (!error) {
        setupTitleWidget();
        mOkButton->setEnabled(true);
    }
}
