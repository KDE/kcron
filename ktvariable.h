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

#include <kdialog.h>

class KTextEdit;

class CTVariable;

/**
  * Environment variable editor window.
  */
class KTVariable : public KDialog
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
  void slotOk();

private:

/**
  * Environment variable.
  */
  CTVariable*     ctvar;

  // Widgets.

  QLabel*         labIcon;

  QLabel*         labVariable;
  QComboBox*      cmbVariable;

  QLabel*         labValue;
  QLineEdit*      leValue;

  QLabel*         labComment;
  KTextEdit*      teComment;

  QCheckBox*      chkEnabled;
};

#endif // KTVARIABLE_H
