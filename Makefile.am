# $Id$
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
#  --------------------------------------------------------------------  #
#  Note: This makefile is for the KDE CVS head only.  To build against   #
#  KDE 1, please download the source from the author's web site at:      #
#  http://24.8.19.113/kcron.html                                         #
##########################################################################

AM_CPPFLAGS = -UQT_NO_ASCII_CAST

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
	ktapp.xpm \
	ktfolder.xpm \
	kthome.xpm \
	ktmail.xpm \
	ktmapp.xpm \
	ktmfolder.xpm \
	ktmhome.xpm \
	ktmmail.xpm \
	ktmpath.xpm \
	ktmshell.xpm \
	ktmsystem.xpm \
	ktmtask.xpm \
	ktmuser.xpm \
	ktmvariable.xpm \
	ktpath.xpm \
	ktshell.xpm \
	ktsystem.xpm \
	kttask.xpm \
	ktuser.xpm \
	ktvariable.xpm \
	ktfilesave.xpm \
	Makefile.am

INCLUDES= $(all_includes)

kcron_METASOURCES = AUTO

kcron_LDFLAGS = $(all_libraries) $(KDE_RPATH)

messages:
	$(XGETTEXT) $(kcron_SOURCES) -o $(podir)/kcron.pot

kcron_LDADD   = $(LIB_KFILE) -lkdeprint

appsdir   = $(kde_appsdir)/System

apps_DATA = kcron.desktop

KDE_ICON = AUTO

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

