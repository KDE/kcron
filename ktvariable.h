/***************************************************************************
 *   ktvariable.h                                                          *
 *   --------------------------------------------------------------------  *
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kaccel.h>
#include <qdialog.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qmultilineedit.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qpushbutton.h>

class CTVariable;

/**
  * Environment variable editor window.
  */
class KTVariable : public QDialog
{
  Q_OBJECT

public:

/**
  * Initialize from CTVariable.
  */
  KTVariable(CTVariable* _ctvar = 0, const QString &_caption="");

/**
  * Destroy.
  */
  ~KTVariable();

private slots:

/**
  * Variable name has been changed.  Refresh icon.
  */
  void slotVariableChanged();

/**
  * Apply changes and close.
  */
  void slotOK();

/**
  * Cancel and close.
  */
  void slotCancel();

private:

/**
  * Auto wrap comment text.
  */
  void autoWrap();

/**
  * Environment variable.
  */
  CTVariable*     ctvar;

/**
  * Key accelerator.
  */

  KAccel*         key_accel;

  // Widgets.

  QLabel*         labIcon;

  QLabel*         labVariable;
  QComboBox*      cmbVariable;

  QLabel*         labValue;
  QLineEdit*      leValue;

  QLabel*         labComment;
  QMultiLineEdit* mleComment;

  QCheckBox*      chkEnabled;

  QButtonGroup*   bgLine;

  QPushButton*    pbOk;
  QPushButton*    pbCancel;

};

#endif // KTVARIABLE_H
