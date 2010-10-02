####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = rars

## INCLUDES were found outside kdevelop specific part

rars_SOURCES = joystick.cpp timer.cpp args.cpp race_data.cpp race_manager.cpp instant_replay.cpp deprecated.cpp trackgen.cpp track.cpp report.cpp movie.cpp misc.cpp drivers.cpp driver_old.cpp driver.cpp carz.cpp 
rars_LDADD   = ./kde/libkde.a ./graphics/libgraphics.a ./robots/librobots.a ./robots/bulle/libbulle.a ./3ds/lib3ds.a ./xml/libxml.a -lGL -lGLU $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) $(LIBSOCKET)

SUBDIRS = kde graphics robots 3ds tracks bitmap xml 

EXTRA_DIST = rars.desktop rars.xpm mini-rars.xpm car.h carz.cpp driver.cpp driver_old.cpp drivers.cpp gi.h help.htm misc.cpp misc.h movie.cpp movie.h os.h report.cpp track.cpp track.h trackgen.cpp rars.ini rars.hlp vecto071.dat douge1.dat bug.dat report.h globals.h draw.h deprecated.cpp deprecated.h instant_replay.cpp instant_replay.h race_manager.cpp race_manager.h race_data.cpp args.cpp timer.cpp timer.h joystick.cpp joystick.h 

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/rars.desktop $(kde_appsdir)/Applications/rars.desktop
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/rars.xpm $(kde_icondir)/hicolor/32x32/apps/rars.xpm
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/mini-rars.xpm $(kde_icondir)/hicolor/16x16/apps/rars.xpm
	$(mkinstalldirs) /usr/share/rars/
	$(INSTALL_DATA) $(srcdir)/rars.ini /usr/share/rars/rars.ini
	$(mkinstalldirs) /usr/share/rars/
	$(INSTALL_DATA) $(srcdir)/rars.hlp /usr/share/rars/rars.hlp
	$(mkinstalldirs) /usr/share/rars/
	$(INSTALL_DATA) $(srcdir)/vecto071.dat /usr/share/rars/vector071.dat
	$(mkinstalldirs) /usr/share/rars/
	$(INSTALL_DATA) $(srcdir)/douge1.dat /usr/share/rars/douge1.dat
	$(mkinstalldirs) /usr/share/rars/
	$(INSTALL_DATA) $(srcdir)/bug.dat /usr/share/rars/bug.dat

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/rars.desktop
	-rm -f $(kde_icondir)/hicolor/32x32/apps/rars.xpm
	-rm -f $(kde_icondir)/hicolor/16x16/apps/rars.xpm
	-rm -f /usr/share/rars/rars.ini
	-rm -f /usr/share/rars/rars.hlp
	-rm -f /usr/share/rars/vector071.dat
	-rm -f /usr/share/rars/douge1.dat
	-rm -f /usr/share/rars/bug.dat

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

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
rars_LDFLAGS = $(all_libraries) $(KDE_RPATH)

rcdir = $(kde_datadir)/rars
rc_DATA = rarsui.rc

messages: rc.cpp
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/rars.pot; \
	fi

