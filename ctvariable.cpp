/***************************************************************************
 *   ctvariable.cpp                                                        *
 *   --------------------------------------------------------------------  *
 *   CT Environment Variable Implementation                                *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include "ctvariable.h"

CTVariable::CTVariable(string tokStr, string _comment)
{
  if (tokStr.substr(0,2) == "#\\")
  {
    tokStr = tokStr.substr(2,tokStr.length() - 2);
    enabled = false;
  }
  else
    enabled = true;

  int spacepos(0);

  spacepos        = tokStr.find_first_of(" =");
  variable        = tokStr.substr(0,spacepos);

  value           = tokStr.substr(spacepos+1,tokStr.length()-spacepos-1);
  comment         = _comment;

  initialVariable = variable;
  initialValue    = value;
  initialComment  = comment;
  initialEnabled  = enabled;
}

CTVariable::CTVariable(const CTVariable &source) :
  variable(source.variable),
  value(source.value),
  comment(source.comment),
  enabled(source.enabled),
  initialVariable(""),
  initialValue(""),
  initialComment(""),
  initialEnabled(true)
{
}

void CTVariable::operator = (const CTVariable& source)
{
  variable        = source.variable;
  value           = source.value;
  comment         = source.comment;
  enabled         = source.enabled;
  initialVariable = "";
  initialValue    = "";
  initialComment  = "";
  initialEnabled  = true;
  return;
}

ostream& operator << (ostream& outputStream, const CTVariable& source)
{
  if (source.comment != string(""))
    outputStream << "# " << source.comment << "\n";

  if (!source.enabled)
    outputStream << "#\\";

  outputStream << source.variable << "=" << source.value << "\n";

  return outputStream;
}

void CTVariable::apply()
{
  initialVariable = variable;
  initialValue    = value;
  initialComment  = comment;
  initialEnabled  = enabled;
}

void CTVariable::cancel()
{
  variable = initialVariable;
  value    = initialValue;
  comment  = initialComment;
  enabled  = initialEnabled;
}

bool CTVariable::dirty() const
{
  return ((variable != initialVariable) || (value != initialValue) ||
    (comment != initialComment) || (enabled != initialEnabled));
}
