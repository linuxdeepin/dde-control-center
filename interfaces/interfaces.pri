
DEFINES += QT_MESSAGELOGCONTEXT

HEADERS += \
    $$PWD/frameproxyinterface.h \
    $$PWD/plugininterface.h \
    $$PWD/moduleinterface.h

SOURCES += \

INCLUDEPATH += $$PWD

isEmpty(PREFIX)
{
    PREFIX = /usr
}
