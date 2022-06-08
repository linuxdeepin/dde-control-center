/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "controlcenterdbusadaptor.h"
#include "mainwindow.h"
#include "interface/moduleobject.h"

#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QThread>
#include <QGuiApplication>
#include <QScreen>
#include <QString>
#include <QWindow>

#include <unistd.h>

DCC_USE_NAMESPACE

/*
 * Implementation of adaptor class DBusControlCenter
 */

ControlCenterDBusAdaptor::ControlCenterDBusAdaptor(MainWindow *parent)
    : QDBusAbstractAdaptor(parent)
{
}

ControlCenterDBusAdaptor::~ControlCenterDBusAdaptor()
{
}

MainWindow *ControlCenterDBusAdaptor::parent() const
{
    return static_cast<MainWindow *>(QObject::parent());
}

const QRect ControlCenterDBusAdaptor::rect() const
{
    return parent()->geometry();
}

void ControlCenterDBusAdaptor::Exit()
{
    pid_t pid = getpid();
    qDebug() << "exit pid:" << pid;
    QCoreApplication::quit();
}

void ControlCenterDBusAdaptor::Hide()
{
    parent()->hide();
}

void ControlCenterDBusAdaptor::Show()
{
    if (parent()->isMinimized() || !parent()->isVisible())
        parent()->show();

    parent()->activateWindow();
}

void ControlCenterDBusAdaptor::ShowHome()
{
    parent()->showPage("", MainWindow::UrlType::Name);
    Show();
}

void ControlCenterDBusAdaptor::ShowPage(const QString &url)
{
    parent()->showPage(url);
    Show();
}

void ControlCenterDBusAdaptor::Toggle()
{
    parent()->setVisible(!parent()->isVisible());
    if (parent()->isVisible())
        parent()->activateWindow();
}

QJsonArray getJsonArray(ModuleObject *module)
{
    QList<ModuleObject *> modules;
    modules.append(module->childrens());
    QJsonArray arr;
    while (!modules.isEmpty()) {
        ModuleObject *obj = modules.takeFirst();
        QJsonObject json;
        json.insert("name", obj->name());
        json.insert("displayName", obj->displayName());
        if (obj->hasChildrens()) {
            json.insert("child", getJsonArray(obj));
        }
        arr.append(json);
    }
    return arr;
}

struct ModuleUrlInfo
{
    ModuleObject *module;
    QString url;
    QString displayName;
};

QString ControlCenterDBusAdaptor::GetAllModule()
{
    ModuleObject *root = parent()->getRootModule();
    QList<ModuleUrlInfo> modules;
    for (auto &&child : root->childrens()) {
        ModuleUrlInfo urlInfo;
        urlInfo.module = child;
        urlInfo.url = child->name();
        urlInfo.displayName = child->displayName();
        modules.append(urlInfo);
    }

    QJsonArray arr;
    while (!modules.isEmpty()) {
        const ModuleUrlInfo &urlInfo = modules.takeFirst();
        QJsonObject obj;
        obj.insert("url", urlInfo.url);
        obj.insert("displayName", urlInfo.displayName);
        arr.append(obj);
        for (auto &&child : urlInfo.module->childrens()) {
            ModuleUrlInfo childUrlInfo;
            childUrlInfo.module = child;
            childUrlInfo.url = urlInfo.url + "/" + child->name();
            childUrlInfo.displayName = urlInfo.displayName + "/" + child->displayName();
            modules.append(childUrlInfo);
        }
    }

    QJsonDocument doc;
    doc.setArray(arr);
    return doc.toJson(QJsonDocument::Compact);
}
