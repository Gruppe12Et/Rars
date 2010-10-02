####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = lib3ds.a

INCLUDES = $(all_includes)

lib3ds_a_METASOURCES = AUTO

lib3ds_a_SOURCES = chunk_3ds.cpp gl_texture.cpp model_3ds.cpp 

SUBDIRS = futura porshe golf 

EXTRA_DIST = model_3ds.cpp model_3ds.h gl_texture.cpp gl_texture.h wheel_front.3ds wheel_rear.3ds helmet.3ds car_body.3ds chunk_3ds.cpp instant_replay.3ds 

install-data-local:
	$(mkinstalldirs) /usr/share/rars/3ds/
	$(INSTALL_DATA) $(srcdir)/wheel_front.3ds /usr/share/rars/3ds/wheel_front.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/
	$(INSTALL_DATA) $(srcdir)/wheel_rear.3ds /usr/share/rars/3ds/wheel_rear.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/
	$(INSTALL_DATA) $(srcdir)/helmet.3ds /usr/share/rars/3ds/helmet.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/
	$(INSTALL_DATA) $(srcdir)/car_body.3ds /usr/share/rars/3ds/car_body.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/
	$(INSTALL_DATA) $(srcdir)/instant_replay.3ds /usr/share/rars/3ds/instant_replay.3ds

uninstall-local:
	-rm -f /usr/share/rars/3ds/wheel_front.3ds
	-rm -f /usr/share/rars/3ds/wheel_rear.3ds
	-rm -f /usr/share/rars/3ds/helmet.3ds
	-rm -f /usr/share/rars/3ds/car_body.3ds
	-rm -f /usr/share/rars/3ds/instant_replay.3ds

####### kdevelop will overwrite this part!!! (end)############
