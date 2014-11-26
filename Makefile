PREFIX = /usr

all: build translate

build:
	mkdir -p out
	cd out; qmake ..; make

translate:
	deepin-generate-mo locale/locale_config.ini

install: build translate
	mkdir -p ${DESTDIR}${PREFIX}/share/locale
	mkdir -p ${DESTDIR}${PREFIX}/share/applications
	mkdir -p ${DESTDIR}/etc/xdg/autostart
	mkdir -p ${DESTDIR}${PREFIX}/share/dbus-1/services

	cp -r locale/mo/* ${DESTDIR}${PREFIX}/share/locale/
	cd out; make INSTALL_ROOT=${DESTDIR} install
	cp dde-control-center.desktop ${DESTDIR}${PREFIX}/share/applications/
	cp dde-dock-applets-autostart.desktop ${DESTDIR}/etc/xdg/autostart/
	cp *.service ${DESTDIR}${PREFIX}/share/dbus-1/services/

clean:
	rm -rf out
