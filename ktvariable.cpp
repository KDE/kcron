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

#include "ktvariable.h"

#include <QLayout>
//Added by qt3to4:
#include <QLabel>
#include <QGridLayout>

#include <klocale.h>  // i18n()
#include <kmessagebox.h>
#include <ktextedit.h>
#include <ktitlewidget.h>

#include "ctvariable.h"
#include "kticon.h"

KTVariable::KTVariable(CTVariable* _ctvar,const QString &_caption) :
  KDialog(),
  ctvar( _ctvar)
{
  setCaption( _caption );
  setButtons( Ok|Cancel );
  setDefaultButton( Ok );
  QWidget *page = new QWidget( this );
  setMainWidget( page );

  QGridLayout *layout = new QGridLayout( page );
  layout->setMargin( 0 );
  layout->setSpacing( spacingHint() );
  layout->setColumnMinimumWidth(1, 270);
  layout->setRowStretch(3, 1);
  layout->setColumnStretch(1, 1);

  setWindowIcon(KTIcon::application(true));

  // top title widget
  titleWidget = new KTitleWidget(page);
  layout->addWidget(titleWidget, 0, 0, 1, 3);

  // variable
  labVariable = new QLabel(i18n("&Variable:"), page );
  labVariable->setObjectName("labVariable");
  layout->addWidget(labVariable, 1, 0, Qt::AlignLeft | Qt::AlignTop);

  cmbVariable = new QComboBox(page);
  cmbVariable->setEditable(true);
  cmbVariable->setObjectName("cmbVariable");
  layout->addWidget(cmbVariable, 1, 1, 1, 2);

  cmbVariable->addItem("HOME");
  cmbVariable->addItem("MAILTO");
  cmbVariable->addItem("PATH");
  cmbVariable->addItem("SHELL");

  labVariable->setBuddy(cmbVariable);

  // value
  labValue = new QLabel(i18n("Va&lue:"), page);
  labValue->setObjectName("labValue");
  layout->addWidget(labValue, 2, 0, Qt::AlignLeft | Qt::AlignTop);

  leValue = new QLineEdit(page);
  leValue->setObjectName("leValue");
  layout->addWidget(leValue, 2, 1, 1, 2);
  leValue->setMaxLength(255);
  labValue->setBuddy(leValue);

  // comment
  labComment = new QLabel(i18n("Co&mment:"), page);
  labComment->setObjectName("labComment");
  layout->addWidget(labComment, 3, 0, Qt::AlignLeft | Qt::AlignTop);

  teComment = new KTextEdit(page);
  layout->addWidget(teComment, 3, 1, 1, 2);
  labComment->setBuddy(teComment);

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), page);
  chkEnabled->setObjectName("chkEnabled");
  layout->addWidget(chkEnabled, 4, 0);

  // set starting field values
  cmbVariable->setEditText(QString::fromLocal8Bit(ctvar->variable.c_str()));
  leValue->setText(QString::fromLocal8Bit(ctvar->value.c_str()));
  teComment->setPlainText(QString::fromLocal8Bit(ctvar->comment.c_str()));
  chkEnabled->setChecked(ctvar->enabled);
  cmbVariable->setFocus();

  slotWizard();

  // connect them up
  connect(cmbVariable,SIGNAL(textChanged(const QString&)), SLOT(slotWizard()));
  connect(leValue,SIGNAL(textChanged(const QString&)), SLOT(slotWizard()));
  connect(this,SIGNAL(okClicked()),this,SLOT(slotOk()));
  connect(chkEnabled, SIGNAL(clicked()), SLOT(slotEnabled()));
}

KTVariable::~KTVariable()
{
}

void KTVariable::setupTitleWidget(const QString &comment)
{
  if (comment == "")
  {
    // try and get an icon for the variable
    QPixmap qpIcon(KTIcon::getMaxIcon("mime_empty"));
    QString variable = cmbVariable->currentText();
    if (variable == "HOME") qpIcon = KTIcon::getMaxIcon("go-home");
    else if (variable == "MAILTO") qpIcon= KTIcon::getMaxIcon("mail");
    else if (variable == "SHELL") qpIcon = KTIcon::getMaxIcon("openterm");
    else if (variable == "PATH") qpIcon = KTIcon::getMaxIcon("folder");
   
    titleWidget->setText(i18n("Add or modify a Kcron variable"));  
    titleWidget->setComment(i18n("<i>This variable has a valid configuration ...</i>"));
    titleWidget->setPixmap(qpIcon, KTitleWidget::ImageRight);
  }  
  else  
  {
    titleWidget->setText(i18n("Kcron Information"));  
    titleWidget->setComment(comment);
    titleWidget->setPixmap(KIcon(KTIcon::getMaxIcon("dialog-information")), KTitleWidget::ImageRight);
  }
}


void KTVariable::slotEnabled()
{
  bool enabled = chkEnabled->isChecked(); 
  labVariable->setEnabled(enabled);
  cmbVariable->setEnabled(enabled);
  labValue->setEnabled(enabled);
  leValue->setEnabled(enabled);
  labComment->setEnabled(enabled);
  teComment->setEnabled(enabled);

  slotWizard();
}


void KTVariable::slotOk()
{
  ctvar->variable = (const char*)cmbVariable->currentText().toLatin1()/*.toLocal8Bit()*/;
  ctvar->value    = (const char*)leValue->text().toLatin1();
  ctvar->comment  = (const char*)teComment->toPlainText().replace('\n',' ').replace('\r',' ').toLatin1();
  ctvar->enabled  = chkEnabled->isChecked();
  close();
}


void KTVariable::slotWizard()
{
  QString variable = cmbVariable->currentText();
  if (variable == "HOME") teComment->setPlainText(i18n("Override default home folder."));
  else if (variable == "MAILTO") teComment->setPlainText(i18n("Email output to specified account."));
  else if (variable == "SHELL") teComment->setPlainText(i18n("Override default shell."));
  else if (variable == "PATH") teComment->setPlainText(i18n("Folders to search for program files."));

  bool error(false);

  if (!chkEnabled->isChecked())
  {
    setupTitleWidget(i18n("<i>Please check 'Enabled' to edit this variable ...</i>"));    
    chkEnabled->setFocus();
    KDialog::enableButtonOk(true);
    error = true;
  }

  if (cmbVariable->currentText().isEmpty())
  {
    setupTitleWidget(i18n("<i>Please enter the variable name ...</i>"));
    cmbVariable->setFocus();
    KDialog::enableButtonOk(false);
    error = true;
  }

  if (leValue->text().isEmpty())
  {
    setupTitleWidget(i18n("<i>Please enter the variable value ...</i>"));
    leValue->setFocus();
    KDialog::enableButtonOk(false);
    error = true;
  }

  if (!error) 
  {
    setupTitleWidget();
    KDialog::enableButtonOk(true);
  }
}

#include "ktvariable.moc"
