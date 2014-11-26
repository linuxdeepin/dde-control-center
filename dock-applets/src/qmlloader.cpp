/****************************************************************************
**
**  Copyright (C) 2011~2014 Deepin, Inc.
**                2011~2014 Kaisheng Ye
**
**  Author:     Kaisheng Ye <kaisheng.ye@gmail.com>
**  Maintainer: Kaisheng Ye <kaisheng.ye@gmail.com>
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <QApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDBusConnection>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QProcess>
#include <QFile>

#include "qmlloader.h"

#undef signals
extern "C" {
  #include <gtk/gtk.h>
}
#define signals public


QmlLoader::QmlLoader(QObject *parent)
    :QObject(parent)
{
    engine = new QQmlEngine(this);
    component = new QQmlComponent(engine, this);
    rootContext = new QQmlContext(engine, this);
    this->m_dbus_proxyer = new AppletDBus(this);
}

QmlLoader::~QmlLoader()
{
    delete this->m_dbus_proxyer;
    delete this->rootContext;
    delete this->component;
    delete this->engine;
}

QString QmlLoader::getIconUrl(QString path)
{
    QString iconPath = "file://"DOCKAPPLETSDIR"/icons/" + path;
    return iconPath;
}

void QmlLoader::xdgOpen(QString path)
{
    QProcess::execute("xdg-open " + path);
}

bool QmlLoader::isPathExist(QString path)
{
    if(path.startsWith("file://")){
        path = path.replace("file://", "");
    }
    return QFile::exists(path);
}

QString QmlLoader::iconNameToPath(QString qname, int size)
{
    char *name = qname.toUtf8().data();

    if (g_path_is_absolute(name))
        return qname;
    g_return_val_if_fail(name != NULL, NULL);

    int pic_name_len = strlen(name);
    char* ext = strrchr(name, '.');
    if (ext != NULL) {
        if (g_ascii_strcasecmp(ext+1, "png") == 0 || g_ascii_strcasecmp(ext+1, "svg") == 0 || g_ascii_strcasecmp(ext+1, "jpg") == 0) {
            pic_name_len = ext - name;
            g_debug("desktop's Icon name should an absoulte path or an basename without extension");
        }
    }

    char* pic_name = g_strndup(name, pic_name_len);
    GtkIconTheme* them = gtk_icon_theme_get_default();

    GtkIconInfo* info = gtk_icon_theme_lookup_icon(them, pic_name, size, GTK_ICON_LOOKUP_GENERIC_FALLBACK);
    g_free(pic_name);
    if (info) {
        char* path = g_strdup(gtk_icon_info_get_filename(info));
#if GTK_MAJOR_VERSION >= 3
        g_object_unref(info);
#elif GTK_MAJOR_VERSION == 2
        gtk_icon_info_free(info);
#endif
        return QString(path);
    } else {
        return NULL;
    }
}

QStringList QmlLoader::scanPlugins()
{
    QStringList result;
    QDir pluginDir(DOCKAPPLETSDIR"/plugins");
    QStringList dlist = pluginDir.entryList();
    for(int i=0;i<dlist.length();i++){
        QString pluginId = dlist[i];
        if(pluginId == "." || pluginId == ".."){
            continue;
        }

        QString tmpPath = DOCKAPPLETSDIR"/plugins/" + pluginId + "/main.qml";
        if(QFile::exists(tmpPath)){
            result.append(pluginId);
            result.append("file://" + tmpPath);
        }
    }
    return result;
}

void QmlLoader::restartDockApplet() {
    QProcess::startDetached(QCoreApplication::applicationFilePath());
    QCoreApplication::exit(0);
}


void QmlLoader::load(QUrl url)
{
    this->component->loadUrl(url);
    this->rootObject = this->component->beginCreate(this->rootContext);
    if ( this->component->isReady() ){
        this->component->completeCreate();
    }
    else{
        qWarning() << this->component->errorString();
    }
}

QString QmlLoader::getAppletInfoListFromQml()
{
    QVariant returnedValue;
    QMetaObject::invokeMethod(
                rootObject,
                "exportAppletStates",
                Q_RETURN_ARG(QVariant, returnedValue)
                );
    return returnedValue.toString();
}

void QmlLoader::setAppletState(int dockMode, QString appletId, bool appletVisible)
{
    QString filePath = QDir::homePath() + "/.dde-dock-applets.ini";
    QSettings sysConfig(filePath, QSettings::IniFormat, 0);

    QString title;
    switch (dockMode) {
    case 1:
        title = "win7";
        break;
    case 2:
        title = "xp";
        break;
    default:
        title = "applet";
        break;
    }

    QString value = appletVisible ? "true":"false";

    sysConfig.beginWriteArray(title);
    sysConfig.setValue(appletId, value);
    sysConfig.endArray();

    Q_EMIT m_dbus_proxyer->appletInfosChanged();
}

bool QmlLoader::getAppletState(int dockMode, QString appletId)
{
    QString filePath = QDir::homePath() + "/.dde-dock-applets.ini";
    QString appletVisible;
    if(QFile::exists(filePath)){
        QString title;
        switch (dockMode) {
        case 1:
            title = "win7";
            break;
        case 2:
            title = "xp";
            break;
        default:
            title = "applet";
            break;
        }
        QSettings sysConfig(filePath, QSettings::IniFormat, 0);
        appletVisible = sysConfig.value("/" + title + "/" + appletId, "true").toString();
    }
    else{
        appletVisible = "true";
    }

    return appletVisible == "true" ? true : false;
}

AppletDBus::AppletDBus(QmlLoader *parent):
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    QDBusConnection::sessionBus().registerObject("/dde/dock/entry/AppletManager", parent);
}

AppletDBus::~AppletDBus()
{

}

QString AppletDBus::appletInfoList()
{
    return m_parent->getAppletInfoListFromQml();
}

void AppletDBus::ShowApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "setAppletState",
                Q_ARG(QVariant, QVariant::fromValue(id)),
                Q_ARG(QVariant, QVariant::fromValue(true))
                );
}

void AppletDBus::HideApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "setAppletState",
                Q_ARG(QVariant, QVariant::fromValue(id)),
                Q_ARG(QVariant, QVariant::fromValue(false))
                );
}

void AppletDBus::ToggleApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "toggleAppletState",
                Q_ARG(QVariant, QVariant::fromValue(id))
                );
}

void AppletDBus::ShowSettingWindow(int mouseX)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "showSettingWindow",
                Q_ARG(QVariant, QVariant::fromValue(mouseX))
                );
}
