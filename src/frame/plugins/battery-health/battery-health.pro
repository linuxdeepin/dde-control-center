#-------------------------------------------------
#
# Project created by QtCreator 2020-11-02T21:25:52
#
#-------------------------------------------------

#手动调试
#更改gsettings配置,注意:voice是辅助功能,我写的二级例子为BatteryHealth,插入电池管理二级菜单最后一个
# gsettings config: {"mainwindow": {"voice": {"enable": true,"index": "keyboard","maxmenu": 3},"Privacy": {"enable": true,"index": "commoninfo","maxmenu": 3}},"Power":{"BatteryHealth":{"enable": true,"index": 3,"maxmenu": 3}}}
# 新建翻译文件目录,复制翻译文件和so库,注意:我们一般把翻译文件指定到一个目录,方便读取
#sudo cp ../translations/battery-health_zh_CN.qm /usr/share/battery-health/translations
#sudo mv ../../libprivacy.so /usr/lib/dde-control-center/modules/

PLUGIN_NAME 	= battery-health

QT              += core gui dbus x11extras svg sql dtkwidget
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dframeworkdbus dtkgui dtkwidget gsettings-qt

TARGET          = battery-health
DESTDIR          = $$_PRO_FILE_PWD_/../

DISTFILES       += $$PLUGIN_NAME.json \
    battery-health.json \

INCLUDEPATH+=/usr/include/dde-control-center

TRANSLATIONS +=\
    translations/battery-health.ts\
    translations/battery-health_zh_CN.ts \

unix {
    target.path = /usr/lib/dde-control-center/modules
    target.file = libbattery-health.so
    translationFile.path = /usr/share/battery-health/translations
    translationFile.files = translations/battery-health_zh_CN.qm
    INSTALLS +=  target translationFile
}

RESOURCES += \
    resource.qrc

HEADERS += \
    batteryhealthplugin.h \
    batteryhealthwidget.h

SOURCES += \
    batteryhealthplugin.cpp \
    batteryhealthwidget.cpp
