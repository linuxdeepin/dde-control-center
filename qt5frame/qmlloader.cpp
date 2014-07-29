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
#include <QDBusInterface>
#include <QProcess>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QGuiApplication>
#include <QPixmap>
#include <QCursor>
#include <QApplication>
#include <QDir>

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

QString QmlLoader::toHumanShortcutLabel(QString sequence)
{
    QStringList sequenceList = sequence.split("-");
    for(int i=0;i<sequenceList.length();i++){
        QString tmpStr = sequenceList[i];
        int tmpStrSize = tmpStr.size();
        sequenceList[i] = tmpStr.left(1).toUpper() + tmpStr.right(tmpStrSize - 1);
    }
    return sequenceList.join("+");
}

void QmlLoader::installPackage(QString packageName)
{
    QString dbus_name = "com.linuxdeepin.softwarecenter_frontend";
    QString dbus_path = "/com/linuxdeepin/softwarecenter_frontend";
    QDBusInterface *iface;
    iface = new QDBusInterface(dbus_name, dbus_path, dbus_name, QDBusConnection::sessionBus());
    iface->call("install_pkgs", QStringList() << packageName);
    iface->call("raise_to_top");
    iface->call("show_page", "install");
}

QString QmlLoader::getGplText(QString language, QString type)
{
    if (!(language == "zh_CN" || language == "zh_TW")){
        language = "en";
    }
    QString gpl_path = APPSHAREDIR"/data/gpl/gpl-3.0-" + language + "-" + type + ".txt";

    QString resultText = "";
    QFile file(gpl_path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return resultText;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        resultText += line + "\n";
    }
    return resultText;
}

void QmlLoader::setCustomCursor(QString path)
{
    path = path.split("//")[1];
    QPixmap pixmap = QPixmap(path);
    QCursor cursor = QCursor(pixmap, -1, -1);
    QGuiApplication::restoreOverrideCursor();
    QGuiApplication::setOverrideCursor(cursor);
    QGuiApplication::changeOverrideCursor(cursor);
}

void QmlLoader::clearCustomCursor()
{
    QGuiApplication::restoreOverrideCursor();
}

void QmlLoader::setCursorFlashTime(int time)
{
    QApplication::setCursorFlashTime(time);
}

QString QmlLoader::getDefaultMask(QString ipAddress)
{
    return "255.255.255.0";
}

QString QmlLoader::getHomeDir()
{
    return QDir::homePath();
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
