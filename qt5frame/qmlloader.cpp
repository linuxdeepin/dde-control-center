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
    this->m_dbus_proxyer = new QmlLoaderDBus(this);
}

QmlLoader::~QmlLoader()
{
    delete this->m_dbus_proxyer;
    delete this->rootContext;
    delete this->component;
    delete this->engine;
}


void QmlLoader::load(QUrl url)
{
    this->component->loadUrl(url);
    this->rootObject = this->component->beginCreate(this->rootContext);
    if ( this->component->isReady() ){
        this->component->completeCreate();
        //QObject::connect(rootObject, SIGNAL(appletInfosChanged()), this, SLOT(appletInfosChangedSlot()));
    }
    else{
        qWarning() << this->component->errorString();
    }
}

void QmlLoader::toggle()
{
    QMetaObject::invokeMethod(
                this->rootObject,
                "togglePanel"
                );
}

void QmlLoader::show()
{
    QVariant second = QVariant(0);
    QMetaObject::invokeMethod(
                this->rootObject,
                "showDss",
                Q_ARG(QVariant, second)
                );
}

void QmlLoader::showModule(QString name)
{
    QVariant moduleName = QVariant(name);
    QMetaObject::invokeMethod(
                this->rootObject,
                "showModule",
                Q_ARG(QVariant, moduleName)
                );
}

void QmlLoader::showImmediately()
{
    QMetaObject::invokeMethod(
                this->rootObject,
                "showDssImmediately"
                );
}

void QmlLoader::hide()
{
    QMetaObject::invokeMethod(
                this->rootObject,
                "hideDss"
                );
}

void QmlLoader::hideImmediately()
{
    QMetaObject::invokeMethod(
                this->rootObject,
                "hideDssImmediately"
                );
}

bool QmlLoader::isNetworkCanShowPassword()
{
    QVariant returnValue;
    QMetaObject::invokeMethod(
                this->rootObject,
                "getNetworkCanShowPassword",
                Q_RETURN_ARG(QVariant, returnValue)
                );
    return returnValue.toBool();
}

QmlLoaderDBus::QmlLoaderDBus(QmlLoader *parent):
    QDBusAbstractAdaptor(parent),
    m_parent(parent)
{
    QDBusConnection::sessionBus().registerObject(DBUS_PATH, parent);
}

QmlLoaderDBus::~QmlLoaderDBus()
{

}

void QmlLoaderDBus::Toggle()
{
    m_parent->toggle();
}

void QmlLoaderDBus::Show()
{
    m_parent->show();
}

void QmlLoaderDBus::ShowModule(QString name)
{
    m_parent->showModule(name);
}

void QmlLoaderDBus::ShowImmediately()
{
    m_parent->showImmediately();
}

void QmlLoaderDBus::Hide()
{
    m_parent->hide();
}

void QmlLoaderDBus::HideImmediately()
{
    m_parent->hideImmediately();
}

bool QmlLoaderDBus::isNetworkCanShowPassword()
{
    return m_parent->isNetworkCanShowPassword();
}
