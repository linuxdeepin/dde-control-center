PREFIX = /usr

all:
	mkdir build; cd build; qmake ..; make -j4;

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	mkdir -p ${DESTDIR}${PREFIX}/share/applications
	mkdir -p ${DESTDIR}${PREFIX}/share/dde-control-center
	mkdir -p ${DESTDIR}${PREFIX}/share/dde-control-center/modules
	cp -r modules/*.so ${DESTDIR}${PREFIX}/share/dde-control-center/modules/
	cp -r modules/icons ${DESTDIR}${PREFIX}/share/dde-control-center/modules/
