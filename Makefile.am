####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = kcron
kcron_SOURCES = kttask.cpp ktvariable.cpp ktview.cpp ktapp.cpp ctvariable.cpp cttask.cpp ctmonth.cpp cthost.cpp ctdow.cpp ctdom.cpp ctcron.cpp main.cpp 
kcron_LDADD   =  -lICE -lkfile -lkdeui -lkdecore -lqt -lXext -lX11 $(LIBSOCKET)

SUBDIRS = 

EXTRA_DIST = main.cpp resource.h kcron.desktop kcron.xpm ctcron.cpp ctcron.h ctdebug.h ctdom.cpp ctdom.h ctdow.cpp ctdow.h ctexception.h cthost.cpp cthost.h cthour.h cti18n.h ctminute.h ctmonth.cpp ctmonth.h cttask.cpp cttask.h ctunit.h ctvariable.cpp ctvariable.h ktapp.h ktapp.cpp ktview.cpp ktview.h ktlistvar.h ktvariable.h ktvariable.cpp kttask.h kttask.cpp kticon.h ktlistcron.h ktlisttask.h 

appsdir   = $(kde_appsdir)/Programs/System
apps_DATA = kcron.desktop
KDE_ICON = kcron

####### kdevelop will overwrite this part!!! (end)############
# this 10 paths are KDE specific. Use them:
# kde_htmldir       Where your docs should go to. (contains lang subdirs)
# kde_appsdir       Where your application file (.kdelnk) should go to. 
# kde_icondir       Where your icon should go to.
# kde_minidir       Where your mini icon should go to.
# kde_datadir       Where you install application data. (Use a subdir)
# kde_locale        Where translation files should go to.(contains lang subdirs)
# kde_cgidir        Where cgi-bin executables should go to.
# kde_confdir       Where config files should go to.
# kde_mimedir       Where mimetypes should go to.
# kde_toolbardir    Where general toolbar icons should go to.
# kde_wallpaperdir  Where general wallpapers should go to.

###########################################################################
#
# For compiling with KDE1:
#
# CPPFLAGS = -DKDE -DKDE1
# kcron_CPPFLAGS = -DKDE -DKDE1
#
# For compiling with KDE CVS head:
#
# CPPFLAGS = -DKDE
# kcron_CPPFLAGS = -DKDE
#
###########################################################################

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

