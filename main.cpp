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

#include <kapplication.h>
#include <klocale.h>
#include "ktapp.h"
#include <iostream.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
 
int main(int argc, char* argv[]) 
{ 
    static const char* description = I18N_NOOP("KDE Task Scheduler");

    KAboutData aboutData("kcron", I18N_NOOP("KCron"), 
      VERSION, description, KAboutData::License_GPL, 
      "(c) 1999-2000, Gary Meyer");

    aboutData.addAuthor("Gary Meyer", 0, "gary@meyer.net");
    aboutData.addAuthor("Robert Berry", 0, "rjmber@ntlworld.com");

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
