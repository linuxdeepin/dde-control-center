
DEFINES += QT_MESSAGELOGCONTEXT

HEADERS += \
    $$PWD/frameproxyinterface.h \
    $$PWD/plugininterface.h

SOURCES += \

INCLUDEPATH += $$PWD

isEmpty(PREFIX)
{
    PREFIX = /usr
}
