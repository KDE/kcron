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

#include <kapp.h>     // kapp
#include <klocale.h>  // i18n()
#include <kmessagebox.h>

#include "ctvariable.h"

#include "kticon.h"

KTVariable::KTVariable(CTVariable* _ctvar) :
  QDialog(0, (const char*)"ktvariable", true, WStyle_DialogBorder)
{
  ctvar = _ctvar;

  setIcon(KTIcon::application(true));

  setCaption(i18n("Edit Variable"));

  const int height = 25;
  const int labelWidth = 80;
  const int editWidth = 260;
  const int vSpacing = 10;
  const int hSpacing = 10;

  const int editX = hSpacing + labelWidth + hSpacing;

  // icon
  labIcon = new QLabel(this, "labIcon");
  labIcon->setGeometry(2*hSpacing+labelWidth+editWidth-32,
    vSpacing, 32, 32);

  // variable
  labVariable = new QLabel(i18n("&Variable:"), this, "labVariable");
  labVariable->setGeometry(hSpacing, 2*vSpacing+32,
    labelWidth, height);

  cmbVariable = new QComboBox(true, this, "cmbVariable");
  cmbVariable->setGeometry(editX, labVariable->y(),
    editWidth, height);

  cmbVariable->insertItem("HOME");
  cmbVariable->insertItem("MAILTO");
  cmbVariable->insertItem("PATH");
  cmbVariable->insertItem("SHELL");

  labVariable->setBuddy(cmbVariable);

  // value
  labValue = new QLabel(i18n("Va&lue:"), this, "labValue");
  labValue->setGeometry(hSpacing, labVariable->y()+height+vSpacing,
    labelWidth, height);

  leValue = new QLineEdit(this, "leValue");
  leValue->setGeometry(editX, labValue->y(), editWidth, height);
  leValue->setMaxLength(255);

  labValue->setBuddy(leValue);

  // comment
  labComment = new QLabel(i18n("Co&mment:"), this, "labComment");
  labComment->setGeometry(hSpacing, labValue->y()+height+vSpacing,
    labelWidth, height);

  mleComment = new QMultiLineEdit(this, "mleComment");
  mleComment->setGeometry(editX, labComment->y(), editWidth, 3*height);

  labComment->setBuddy(mleComment);

  // enabled
  chkEnabled = new QCheckBox(i18n("&Enabled"), this, "chkEnabled");
  chkEnabled->setGeometry(hSpacing,
    mleComment->y()+mleComment->height()+vSpacing,
    labelWidth, height);

  // line
  bgLine = new QButtonGroup(this, "bgLine");
  bgLine->setGeometry(hSpacing,
    chkEnabled->y()+chkEnabled->height()+vSpacing,
    labelWidth + hSpacing + editWidth, 2);

  // OK
  pbOk = new QPushButton(i18n("&OK"),this,"pbOk");
  pbOk->setGeometry(bgLine->x()+bgLine->width()-2*labelWidth-hSpacing,
    bgLine->y()+bgLine->height()+vSpacing,
    labelWidth, height);
  pbOk->setDefault(true);

  // Cancel
  pbCancel = new QPushButton(i18n("&Cancel"),this,"pbCancel");
  pbCancel->setGeometry(bgLine->x()+bgLine->width()-labelWidth,
    bgLine->y()+bgLine->height()+vSpacing,
    labelWidth, height);

  // window
  setFixedSize(pbCancel->x()+pbCancel->width()+hSpacing,
    pbCancel->y()+pbCancel->height()+vSpacing);


  // set starting field values
  cmbVariable->setEditText(ctvar->variable.c_str());
  slotVariableChanged();

  leValue->setText(ctvar->value.c_str());

  mleComment->setText(ctvar->comment.c_str());
  autoWrap();

  chkEnabled->setChecked(ctvar->enabled);

  cmbVariable->setFocus();

  // connect them up
  connect(cmbVariable,SIGNAL(highlighted(const QString&)),
    SLOT(slotVariableChanged()));
  connect(cmbVariable,SIGNAL(activated(const QString&)),
    SLOT(slotVariableChanged()));
  connect(pbOk,SIGNAL(clicked()),SLOT(slotOK()));
  connect(pbCancel,SIGNAL(clicked()),SLOT(slotCancel()));

  // key acceleration

  key_accel = new KAccel(this);

  key_accel->connectItem(KStdAccel::Open, this, SLOT(slotOK()));
  key_accel->connectItem(KStdAccel::Close, this, SLOT(slotCancel()));
  key_accel->connectItem(KStdAccel::Quit, this, SLOT(slotCancel()));
  key_accel->readSettings();

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

void KTVariable::slotOK()
{
  if (QString(cmbVariable->currentText()) == "")
  {
    KMessageBox::information(this, i18n("Please enter the variable name."));
    cmbVariable->setFocus();
    return;
  }

  ctvar->variable = cmbVariable->currentText();
  ctvar->value    = leValue->text();
  ctvar->comment  = mleComment->text();
  ctvar->enabled  = chkEnabled->isChecked();
  close();
}

void KTVariable::slotCancel()
{
  close();
}

void KTVariable::autoWrap()
{
  int maxCharWidth = (int)(cmbVariable->width() /
    ((mleComment->maxLineWidth()*1.05) /
    mleComment->text().length()));

  unsigned int pos(maxCharWidth);
  QString s(ctvar->comment.c_str());

  while(pos < s.length())
  {
    int foundat = s.findRev(" ", pos);
    if (foundat);
      s.replace(foundat, 1, "\n");
    pos = foundat+maxCharWidth;
  }

  mleComment->setText(s);

  mleComment->setCursorPosition(mleComment->numLines()-1,
    mleComment->maxLineWidth()-1);
}



