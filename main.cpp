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
#include "ktapp.h"
 
int main(int argc, char* argv[]) { 

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
