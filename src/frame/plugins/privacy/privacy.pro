#-------------------------------------------------
#
# Project created by QtCreator 2020-11-02T21:25:52
#
#-------------------------------------------------

#手动调试
#更改gsettings配置,注意:voice是辅助功能,我写的一级例子为Privacy
# gsettings config: {"mainwindow": {"voice": {"enable": true,"index": "keyboard","maxmenu": 3},"Privacy": {"enable": true,"index": "commoninfo","maxmenu": 3}}}
# 复制翻译文件和so库,注意:我们一般把翻译文件指定到一个目录,方便读取
#sudo cp ../translations/privacy-plugin_zh_CN.qm /usr/share/privacy-plugin/translations
#sudo mv ../../libprivacy.so /usr/lib/dde-control-center/modules/

PLUGIN_NAME 	= privacy

QT              += core gui dbus x11extras svg sql dtkwidget5.5
TEMPLATE         = lib
CONFIG          += plugin c++11 link_pkgconfig
PKGCONFIG       += dframeworkdbus dtkgui5.5 dtkwidget5.5 gsettings-qt

TARGET          = privacy
DESTDIR          = $$_PRO_FILE_PWD_/../

DISTFILES       += $$PLUGIN_NAME.json \
    notify.json \

INCLUDEPATH+=/usr/include/dde-control-center

TRANSLATIONS +=\
    translations/privacy-plugin.ts\
    translations/privacy-plugin_zh_CN.ts \

unix {
    target.path = /usr/lib/dde-control-center/modules
    target.file = libprivacy.so
    translationFile.path = /usr/share/privacy-plugin/translations
    translationFile.files = translations/privacy-plugin_zh_CN.qm
    INSTALLS +=  target translationFile
}

RESOURCES += \
    resource.qrc

HEADERS += \
    privacyplugin.h \
    privacywidget.h \
    locationwidget.h

SOURCES += \
    privacyplugin.cpp \
    privacywidget.cpp \
    locationwidget.cpp
