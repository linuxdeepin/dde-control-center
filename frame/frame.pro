QT += widgets

HEADERS    = frame.h \
             interfaces.h \
    homescreen.h \
    modulemetadata.h
SOURCES    = frame.cpp \
             main.cpp \
    homescreen.cpp

TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../
