/***************************************************************************
 *   ktvariable.cpp                                                        *
 *   --------------------------------------------------------------------  *
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

#include <qlayout.h>
#include <kapplication.h>     // kapp
#include <klocale.h>  // i18n()
#include <kmessagebox.h>

#include "ctvariable.h"

#include "kticon.h"

KTVariable::KTVariable(CTVariable* _ctvar,const QString &_caption) :
  KDialogBase(0, "ktvariable", true, _caption, Ok|Cancel, Ok, true),
  ctvar( _ctvar)
{
  QFrame *page = makeMainWidget();
  QGridLayout *layout = new QGridLayout( page, 5, 3, 0, spacingHint() );
  layout->setRowStretch(3, 1);
  layout->setColStretch(1, 1);

  setIcon(KTIcon::application(true));

  // variable
  labVariable = new QLabel(i18n("&Variable:"), page, "labVariable");
  layout->addWidget(labVariable, 1, 0, Qt::AlignLeft | Qt::AlignTop);

  cmbVariable = new QComboBox(true, page, "cmbVariable");
  layout->addWidget(cmbVariable, 1, 1);

  cmbVariable->insertItem("HOME");
  cmbVariable->insertItem("MAILTO");
  cmbVariable->insertItem("PATH");
  cmbVariable->insertItem("SHELL");

  labVariable->setBuddy(cmbVariable);

  // icon
  labIcon = new QLabel(page, "labIcon");
  layout->addMultiCellWidget(labIcon, 0, 1, 2, 2);

  // value
  labValue = new QLabel(i18n("Va&lue:"), page, "labValue");
  layout->addWidget(labValue, 2, 0, Qt::AlignLeft | Qt::AlignTop);

  leValue = new QLineEdit(page, "leValue");
  layout->addMultiCellWidget(leValue, 2, 2, 1, 2);
  leValue->setMaxLength(255);
  labValue->setBuddy(leValue);

  // comment
  labComment = new QLabel(i18n("Co&mment:"), page, "labComment");
  layout->addWidget(labComment, 3, 0, Qt::AlignLeft | Qt::AlignTop);

  mleComment = new QMultiLineEdit(page, "mleComment");
  layout->addMultiCellWidget(mleComment, 3, 3, 1, 2);

  labComment->setBuddy(mleComment);

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), page, "chkEnabled");
  layout->addWidget(chkEnabled, 4, 0);

  // set starting field values
  cmbVariable->setEditText(QString::fromLocal8Bit(ctvar->variable.c_str()));
  slotVariableChanged();

  leValue->setText(QString::fromLocal8Bit(ctvar->value.c_str()));

  mleComment->setText(QString::fromLocal8Bit(ctvar->comment.c_str()));
  autoWrap();

  chkEnabled->setChecked(ctvar->enabled);

  cmbVariable->setFocus();

  // connect them up
  connect(cmbVariable,SIGNAL(highlighted(const QString&)),
    SLOT(slotVariableChanged()));
  connect(cmbVariable,SIGNAL(activated(const QString&)),
    SLOT(slotVariableChanged()));
}

KTVariable::~KTVariable()
{
}

void KTVariable::slotVariableChanged()
{
  QString variable = cmbVariable->currentText();
  if (variable == "HOME")
  {
    labIcon->setPixmap(KTIcon::home(false));
    mleComment->setText(i18n("Override default home directory."));
  }
  else if (variable == "MAILTO")
  {
    labIcon->setPixmap(KTIcon::mail(false));
    mleComment->setText(i18n("Email output to specified account."));
  }
  else if (variable == "SHELL")
  {
    labIcon->setPixmap(KTIcon::shell(false));
    mleComment->setText(i18n("Override default shell."));
  }
  else if (variable == "PATH")
  {
    labIcon->setPixmap(KTIcon::path(false));
    mleComment->setText(i18n("Directories to search for program files."));
  }
  else
  {
    labIcon->setPixmap(KTIcon::variable(false));
  }
}

void KTVariable::slotOk()
{
  if (QString(cmbVariable->currentText()) == "")
  {
    KMessageBox::information(this, i18n("Please enter the variable name."));
    cmbVariable->setFocus();
    return;
  }

  if (QString(leValue->text()) == "")
  {
    KMessageBox::information(this, i18n("Please enter the variable value."));
    cmbVariable->setFocus();
    return;
  }

  ctvar->variable = cmbVariable->currentText().local8Bit();
  ctvar->value    = leValue->text().local8Bit();
  ctvar->comment  = mleComment->text().local8Bit();
  ctvar->enabled  = chkEnabled->isChecked();
  close();
}

void KTVariable::autoWrap()
{
  int maxCharWidth = (int)(cmbVariable->width() /
    ((mleComment->totalWidth()*1.05) /
    mleComment->text().length()));

  unsigned int pos(maxCharWidth);
  QString s = QString::fromLocal8Bit(ctvar->comment.c_str());

  while(pos < s.length())
  {
    int foundat = s.findRev(" ", pos);
    if (foundat);
      s.replace(foundat, 1, "\n");
    pos = foundat+maxCharWidth;
  }

  mleComment->setText(s);

  mleComment->setCursorPosition(mleComment->numLines()-1,
    mleComment->totalWidth()-1);
}

#include "ktvariable.moc"
