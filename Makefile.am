##########################################################################
#  Makefile.am                                                           #
#  --------------------------------------------------------------------  #
#  Makefile.am for KCron                                                 #
#  --------------------------------------------------------------------  #
#  Copyright (C) 1999, Gary Meyer <gary@meyer.net>                       #
#  --------------------------------------------------------------------  #
#  This program is free software; you can redistribute it and/or modify  #
#  it under the terms of the GNU General Public License as published by  #
#  the Free Software Foundation; either version 2 of the License, or     #
#  (at your option) any later version.                                   # 
##########################################################################

bin_PROGRAMS = kcron

kcron_SOURCES = \
	cthost.cpp \
	ctcron.cpp \
	ctmonth.cpp \
	ctdom.cpp \
	ctdow.cpp \
	cttask.cpp \
	ctvariable.cpp \
	kticon.cpp \
	ktapp.cpp \
	ktview.cpp \
	ktlistitem.cpp \
	ktlistcron.cpp \
	ktlisttask.cpp \
	ktlisttasks.cpp \
	ktlistvar.cpp \
	ktlistvars.cpp \
	kttask.cpp \
	ktvariable.cpp \
	ktprint.cpp \
	ktprintopt.cpp \
	main.cpp

EXTRA_DIST = \
	ctcron.cpp \
	ctcron.h \
	ctdom.cpp \
	ctdom.h \
	ctdow.cpp \
	ctdow.h \
	ctexception.h \
	cthost.cpp \
	cthost.h \
	cthour.h \
	cti18n.h \
	ctminute.h \
	ctmonth.cpp \
	ctmonth.h \
	cttask.cpp \
	cttask.h \
	ctunit.cpp \
	ctunit.h \
	ctvariable.cpp \
	ctvariable.h \
	ktapp.h \
	ktapp.cpp \
	kticon.h \
	kticon.cpp \
	kttask.h \
	kttask.cpp \
	ktvariable.h \
	ktvariable.cpp \
	ktview.cpp \
	ktview.h \
	ktlistitem.h \
	ktlistitem.cpp \
	ktlistcron.h \
	ktlistcron.cpp \
	ktlisttask.h \
	ktlisttask.cpp \
	ktlisttasks.h \
	ktlisttasks.cpp \
	ktlistvar.h \
	ktlistvar.cpp \
	ktlistvars.h \
	ktlistvars.cpp \
	ktprint.h \
	ktprint.cpp \
	ktprintopt.h \
	ktprintopt.cpp \
        main.cpp \
	README \
	AUTHORS \
	COPYING \
	ChangeLog \
	TODO \
	kcron.desktop \
	kcron.lsm \
	Makefile.am

INCLUDES= $(all_includes)

kcron_METASOURCES = AUTO

kcron_LDFLAGS = $(all_libraries) $(KDE_RPATH)

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/kcron.pot

kcron_LDADD   = $(LIB_KFILE) -lkdeprint

xdg_apps_DATA = kcron.desktop

KDE_ICON = kcron
shellrcdir   = $(kde_datadir)/kcron
shellrc_DATA = kcronui.rc
