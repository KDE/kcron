/***************************************************************************
 *   main.cpp                                                              *
 *   --------------------------------------------------------------------  *
 *   Main.                                                                 *
 *   --------------------------------------------------------------------  *
 *   Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       *
 *   --------------------------------------------------------------------  *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 ***************************************************************************/

#include <kapp.h>
#include <klocale.h>
#include "ktapp.h"
#include <iostream.h>

class CTExceptionParse;
class CTExceptionIO;
 
int main(int argc, char* argv[]) { 

  try 
  {
    KApplication app(argc,argv,"kcron");
 
    if (app.isRestored())
      { 
        RESTORE(KTApp);
      }
    else 
      {
        KTApp* kcron = new KTApp;
        kcron->show();
      }
    return app.exec();
  }

  catch (CTExceptionParse& ctep)
  {
    cerr << i18n("KCron fatal error: Unable to parse file.") << endl;
    return 1;
  }
  catch (CTExceptionIO& ctio)
  {
    cerr << i18n("KCron fatal error: Unable to read or write file.") << endl;
    return 1;
  }
  catch (...)
  {
    cerr << i18n("KCron fatal error: Unknown.") << endl;
    return 1;
  }

  return 0;
}
