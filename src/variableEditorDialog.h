/*
    KT environment variable editor window header
    --------------------------------------------------------------------
    SPDX-FileCopyrightText: 1999 Gary Meyer <gary@meyer.net>
    --------------------------------------------------------------------
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>

#include <KTitleWidget>

class CTVariable;
class CrontabWidget;

/**
 * Environment variable editor window.
 */
class VariableEditorDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * Initialize from CTVariable.
     */
    explicit VariableEditorDialog(CTVariable *_ctVariable, const QString &_caption, CrontabWidget *crontabWidget);

    /**
     * Destroy.
     */
    ~VariableEditorDialog() override;

private Q_SLOTS:

    /**
     * Setup title widget
     */
    void setupTitleWidget(const QString &comment = QLatin1String(""), KTitleWidget::MessageType = KTitleWidget::PlainMessage);

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
    CTVariable *mCtVariable = nullptr;

    CrontabWidget *mCrontabWidget = nullptr;

    // Widgets.
    KTitleWidget *mTitleWidget = nullptr;
    QPushButton *mOkButton = nullptr;

    QComboBox *mCmbVariable = nullptr;

    QLabel *mDetailsIcon = nullptr;
    QLabel *mDetails = nullptr;

    QLineEdit *mLeValue = nullptr;

    QTextEdit *mTeComment = nullptr;

    QCheckBox *mChkEnabled = nullptr;

    QComboBox *mUserCombo = nullptr;
};

