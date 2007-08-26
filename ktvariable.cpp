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

#include <kcombobox.h>
#include <klineedit.h>
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

  setWindowIcon(KTIcon::application(KTIcon::Small));

  // top title widget
  titleWidget = new KTitleWidget(page);
  layout->addWidget(titleWidget, 0, 0, 1, 2);

  // variable
  labVariable = new QLabel(i18nc("The environmental variable name ie HOME, MAILTO etc", "&Variable:"), page );
  labVariable->setObjectName("labVariable");
  layout->addWidget(labVariable, 1, 0, Qt::AlignLeft | Qt::AlignTop);

  cmbVariable = new KComboBox(page);
  cmbVariable->setEditable(true);
  cmbVariable->setObjectName("cmbVariable");
  layout->addWidget(cmbVariable, 1, 1, 1, 1);

  cmbVariable->addItem("HOME");
  cmbVariable->addItem("MAILTO");
  cmbVariable->addItem("PATH");
  cmbVariable->addItem("SHELL");

  labVariable->setBuddy(cmbVariable);

  // value
  labValue = new QLabel(i18n("Va&lue:"), page);
  labValue->setObjectName("labValue");
  layout->addWidget(labValue, 2, 0, Qt::AlignLeft | Qt::AlignTop);

  leValue = new KLineEdit(page);
  leValue->setObjectName("leValue");
  layout->addWidget(leValue, 2, 1, 1, 1);
  leValue->setMaxLength(255);
  labValue->setBuddy(leValue);

  // comment
  labComment = new QLabel(i18n("Co&mment:"), page);
  labComment->setObjectName("labComment");
  layout->addWidget(labComment, 3, 0, Qt::AlignLeft | Qt::AlignTop);

  teComment = new KLineEdit(page);
  layout->addWidget(teComment, 3, 1, 1, 1);
  labComment->setBuddy(teComment);

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), page);
  chkEnabled->setObjectName("chkEnabled");
  layout->addWidget(chkEnabled, 4, 0);

  // set starting field values
  cmbVariable->setEditText(QString::fromLocal8Bit(ctvar->variable.c_str()));
  leValue->setText(QString::fromLocal8Bit(ctvar->value.c_str()));
  teComment->setText(QString::fromLocal8Bit(ctvar->comment.c_str()));
  chkEnabled->setChecked(ctvar->enabled);
  cmbVariable->setFocus();

  slotEnabled();
  slotWizard();
  show();

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
  if (comment.isEmpty())	//krazy:exclude=duoblequote_chars
  {
    // try and get an icon for the variable
    QPixmap qpIcon(KTIcon::variable(KTIcon::Large));
    QString variable = cmbVariable->currentText();
    if (variable == "HOME") qpIcon = KTIcon::home(KTIcon::Large);
    else if (variable == "MAILTO") qpIcon= KTIcon::mail(KTIcon::Large);
    else if (variable == "SHELL") qpIcon = KTIcon::shell(KTIcon::Large);
    else if (variable == "PATH") qpIcon = KTIcon::variables(KTIcon::Large);
   
    titleWidget->setText(i18n("Add or modify a Kcron variable"));  
    titleWidget->setComment(i18n("<i>This variable has a valid configuration ...</i>"));
    titleWidget->setPixmap(qpIcon, KTitleWidget::ImageRight);
  }  
  else  
  {
    titleWidget->setText(i18n("Kcron Information"));  
    titleWidget->setComment(comment);
    titleWidget->setPixmap(KIcon(KTIcon::information(KTIcon::Large)), KTitleWidget::ImageRight);
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
}


void KTVariable::slotOk()
{
  ctvar->variable = (const char*)cmbVariable->currentText().toLatin1()/*.toLocal8Bit()*/;
  ctvar->value    = (const char*)leValue->text().toLatin1();
  ctvar->comment  = (const char*)teComment->originalText().replace('\n',' ').replace('\r',' ').toLatin1();
  ctvar->enabled  = chkEnabled->isChecked();
  close();
}


void KTVariable::slotWizard()
{
  QString variable = cmbVariable->currentText();
  if (variable == "HOME") teComment->setText(i18n("Override default home folder."));
  else if (variable == "MAILTO") teComment->setText(i18n("Email output to specified account."));
  else if (variable == "SHELL") teComment->setText(i18n("Override default shell."));
  else if (variable == "PATH") teComment->setText(i18n("Folders to search for program files."));

  bool error(false);

  if (!chkEnabled->isChecked())
  {
    setupTitleWidget(i18n("<i>Please check 'Enabled' to edit this variable ...</i>"));    
    chkEnabled->setFocus();
    KDialog::enableButtonOk(true);
    error = true;
  }

  if (cmbVariable->currentText().isEmpty() && !error)
  {
    setupTitleWidget(i18n("<i>Please enter the variable name ...</i>"));
    cmbVariable->setFocus();
    KDialog::enableButtonOk(false);
    error = true;
  }

  if (leValue->text().isEmpty() && !error)
  {
    setupTitleWidget(i18n("<i>Please enter the variable value ...</i>"));
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
