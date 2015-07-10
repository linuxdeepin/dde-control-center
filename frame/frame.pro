QT += widgets

HEADERS    = frame.h \
             interfaces.h \
    homescreen.h \
    modulemetadata.h \
    constants.h
SOURCES    = frame.cpp \
             main.cpp \
    homescreen.cpp

TARGET     = dde-control-center
DESTDIR    = $$_PRO_FILE_PWD_/../

CONFIG += c++11
