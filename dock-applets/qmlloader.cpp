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


#include <QtGui/QGuiApplication>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QDBusConnection>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSettings>

#include "qmlloader.h"

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
        QObject::connect(rootObject, SIGNAL(appletInfosChanged()), this, SLOT(appletInfosChangedSlot()));
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
                "get_applet_infos",
                Q_RETURN_ARG(QVariant, returnedValue)
                );
    return returnedValue.toString();
}

void QmlLoader::setAppletVisibleToConfig(QString info)
{
    QString filePath = QDir::homePath() + "/.dde-dock-applets.ini";
    QSettings sysConfig(filePath, QSettings::IniFormat, 0);
    sysConfig.beginWriteArray("applet");
    sysConfig.setValue("info", info);
    sysConfig.endArray();
}

QString QmlLoader::getAppletVisibleFromConfig()
{
    QString filePath = QDir::homePath() + "/.dde-dock-applets.ini";
    QString strRst;
    if(QFile::exists(filePath)){
        QSettings sysConfig(filePath, QSettings::IniFormat, 0);
        strRst = sysConfig.value("/applet/info", "").toString();
    }
    else{
        strRst = "";
    }

    return strRst;
}

void QmlLoader::appletInfosChangedSlot()
{
    Q_EMIT m_dbus_proxyer->appletInfosChanged();
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

void AppletDBus::ShowApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "set_show_applet",
                Q_ARG(QVariant, QVariant::fromValue(id))
                );
}

void AppletDBus::HideApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "set_hide_applet",
                Q_ARG(QVariant, QVariant::fromValue(id))
                );
}

void AppletDBus::ToggleApplet(QString id)
{
    QMetaObject::invokeMethod(
                m_parent->rootObject,
                "toggle_applet",
                Q_ARG(QVariant, QVariant::fromValue(id))
                );
}
