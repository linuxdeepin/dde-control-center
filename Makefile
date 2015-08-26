PREFIX = /usr

all:
	mkdir build; cd build; qmake ..; make -j4;

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${PREFIX}/share/dbus-1/services/
	mkdir -p ${DESTDIR}${PREFIX}/share/applications
	mkdir -p ${DESTDIR}${PREFIX}/share/dde-control-center
	mkdir -p ${DESTDIR}${PREFIX}/share/dde-control-center/modules
	cp -r dde-control-center.desktop ${DESTDIR}${PREFIX}/share/applications/
	cp -r com.deepin.dde.ControlCenter.service ${DESTDIR}${PREFIX}/share/dbus-1/services/
	cp -r dde-control-center ${DESTDIR}${PREFIX}/bin/
	cp -r modules/*.so ${DESTDIR}${PREFIX}/share/dde-control-center/modules/
	cp -r modules/icons ${DESTDIR}${PREFIX}/share/dde-control-center/modules/
