TEMPLATE = lib
TARGET = appletwidgets
QT += qml quick
CONFIG += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = Deepin.AppletWidgets 

installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)

qmldir.files += *.qml qmldir images
qmldir.path = $$installPath

target.path = $$installPath

INSTALLS += qmldir target
