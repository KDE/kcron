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
#include <kcmdlineargs.h>
#include <kaboutdata.h>

class CTExceptionIO;
 
int main(int argc, char* argv[]) 
{ 
  try 
  {
    static const char* description = I18N_NOOP("KDE Task Scheduler");

    KAboutData aboutData("kcron", I18N_NOOP("KCron"), 
      VERSION, description, KAboutData::License_GPL, 
      "(c) 1999-2000, Gary Meyer");

    aboutData.addAuthor("Gary Meyer", 0, "gary@meyer.net");

    KCmdLineArgs::init(argc, argv, &aboutData);
  
    KApplication app(&aboutData);
 
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
