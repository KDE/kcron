bin_PROGRAMS = kcron
kcron_SOURCES = kttask.cpp ktvariable.cpp ktview.cpp ktapp.cpp ctvariable.cpp cttask.cpp ctmonth.cpp cthost.cpp ctdow.cpp ctdom.cpp ctcron.cpp main.cpp 
kcron_LDADD   =  -lICE -lkfile -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

SUBDIRS = docs

EXTRA_DIST = main.cpp resource.h kcron.desktop kcron.xpm ctcron.cpp ctcron.h ctdebug.h ctdom.cpp ctdom.h ctdow.cpp ctdow.h ctexception.h cthost.cpp cthost.h cthour.h cti18n.h ctminute.h ctmonth.cpp ctmonth.h cttask.cpp cttask.h ctunit.h ctvariable.cpp ctvariable.h ktapp.h ktapp.cpp ktview.cpp ktview.h ktlistvar.h ktvariable.h ktvariable.cpp kttask.h kttask.cpp kticon.h ktlistcron.h ktlisttask.h 

appsdir   = $(kde_appsdir)/System
apps_DATA = kcron.desktop
KDE_ICON = kcron

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
CPPFLAGS = -DKDE
kcron_CPPFLAGS = -DKDE

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)
# claim, which subdirectories you want to install
# you can add here more. This one gets installed 
bin_PROGRAMS = kcron 

kcron_METASOURCES = USE_AUTOMOC

# the library search path. 
kcron_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# them while "make clean", use CLEANFILES
DISTCLEANFILES = $(kcron_METASOURCES)

# make messages.po. Move this one to ../po/ and "make merge" in po
# the -x is for skipping messages already translated in kdelibs
messages:
	$(XGETTEXT) -C -ki18n -x $(includedir)/kde.pot $(kcron_SOURCES) -o ../po/kcron.pot

