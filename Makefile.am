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
	main.cpp

SUBDIRS = docs 

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
	Makefile.am

INCLUDES= $(all_includes)

kcron_METASOURCES = USE_AUTOMOC

kcron_LDFLAGS = $(all_libraries) $(KDE_RPATH)

DISTCLEANFILES = $(kcron_METASOURCES)

messages:
	$(XGETTEXT) -C -kI18N_NOOP -ki18n -x $(includedir)/kde.pot $(kcron_SOURCES) -o $(podir)/kcron.pot

kcron_LDADD   =  -lICE -lkfile -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

appsdir   = $(kde_appsdir)/System

apps_DATA = kcron.desktop

KDE_ICON = kcron

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
CPPFLAGS = -DKDE
kcron_CPPFLAGS = -DKDE

