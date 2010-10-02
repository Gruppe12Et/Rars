####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = back_right_wheel.3ds car_body.3ds front_right_wheel.3ds golf.3ds 

install-data-local:
	$(mkinstalldirs) /usr/share/rars/3ds/golf/
	$(INSTALL_DATA) $(srcdir)/back_right_wheel.3ds /usr/share/rars/3ds/golf/back_right_wheel.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/golf/
	$(INSTALL_DATA) $(srcdir)/car_body.3ds /usr/share/rars/3ds/golf/car_body.3ds
	$(mkinstalldirs) /usr/share/rars/3ds/golf/
	$(INSTALL_DATA) $(srcdir)/front_right_wheel.3ds /usr/share/rars/3ds/golf/front_right_wheel.3ds

uninstall-local:
	-rm -f /usr/share/rars/3ds/golf/back_right_wheel.3ds
	-rm -f /usr/share/rars/3ds/golf/car_body.3ds
	-rm -f /usr/share/rars/3ds/golf/front_right_wheel.3ds

####### kdevelop will overwrite this part!!! (end)############
