/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     fanpengcheng <fanpengcheng@uniontech.com>
*
* Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QTimer>
#include <QApplication>

const static QString DaemonDockService = "com.deepin.dde.daemon.Dock";
const static QString DaemonDockPath = "/com/deepin/dde/daemon/Dock";
const static QString DaemonDockInterface = "com.deepin.dde.daemon.Dock";
const static QString DockService = "com.deepin.dde.Dock";
const static QString DockPath = "/com/deepin/dde/Dock";
const static QString DockInterface = "com.deepin.dde.Dock";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

DBusProxy::DBusProxy(QObject *parent)
    : QObject(parent)
    , m_daemonDockInter(new QDBusInterface(DaemonDockService, DaemonDockPath, DaemonDockInterface, QDBusConnection::sessionBus(), this))
    , m_dockInter(new QDBusInterface(DockService, DockPath, DockInterface, QDBusConnection::sessionBus(), this))
{
    QDBusConnection::sessionBus().connect(DaemonDockService, DaemonDockPath, PropertiesInterface, PropertiesChanged,this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, PropertiesInterface, PropertiesChanged,this, SLOT(onPropertiesChanged(QDBusMessage)));

    // 对于普通信号需要手动关联
    m_dockInter->connect(m_dockInter, SIGNAL(pluginVisibleChanged(const QString &, bool)), this, SIGNAL(pluginVisibleChanged(const QString &, bool)));
}

int DBusProxy::displayMode()
{
    return qvariant_cast<int>(m_daemonDockInter->property("DisplayMode"));
}

void DBusProxy::setDisplayMode(int mode)
{
    m_daemonDockInter->setProperty("DisplayMode", QVariant::fromValue(mode));
}

int DBusProxy::position()
{
    return qvariant_cast<int>(m_daemonDockInter->property("Position"));
}

void DBusProxy::setPosition(int value)
{
    m_daemonDockInter->setProperty("Position", QVariant::fromValue(value));
}

int DBusProxy::hideMode()
{
    return qvariant_cast<int>(m_daemonDockInter->property("HideMode"));
}

void DBusProxy::setHideMode(int value)
{
    m_daemonDockInter->setProperty("HideMode", QVariant::fromValue(value));
}

uint DBusProxy::windowSizeEfficient()
{
    return qvariant_cast<uint>(m_daemonDockInter->property("WindowSizeEfficient"));
}

void DBusProxy::setWindowSizeEfficient(uint value)
{
    m_daemonDockInter->setProperty("WindowSizeEfficient", QVariant::fromValue(value));
}

uint DBusProxy::windowSizeFashion()
{
    return qvariant_cast<uint>(m_daemonDockInter->property("WindowSizeFashion"));
}

void DBusProxy::setWindowSizeFashion(uint value)
{
    m_daemonDockInter->setProperty("WindowSizeFashion", QVariant::fromValue(value));
}

bool DBusProxy::showInPrimary()
{
    return qvariant_cast<bool>(m_dockInter->property("showInPrimary"));
}

void DBusProxy::setShowInPrimary(bool value)
{
    m_dockInter->setProperty("showInPrimary", QVariant::fromValue(value));
}

void DBusProxy::resizeDock(int offset, bool dragging)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(offset) << QVariant::fromValue(dragging);
    m_dockInter->asyncCallWithArgumentList(QStringLiteral("resizeDock"), argumentList);
}

QDBusPendingReply<QStringList> DBusProxy::GetLoadedPlugins()
{
    QDBusPendingReply<QStringList> reply = m_dockInter->asyncCall(QStringLiteral("GetLoadedPlugins"));
    reply.waitForFinished();
    return reply;
}

QDBusPendingReply<QString> DBusProxy::getPluginKey(const QString &pluginName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("getPluginKey"), argumentList);
}

QDBusPendingReply<bool> DBusProxy::getPluginVisible(const QString &pluginName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("getPluginVisible"), argumentList);
}

QDBusPendingReply<> DBusProxy::setPluginVisible(const QString &pluginName, bool visible)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName) << QVariant::fromValue(visible);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("setPluginVisible"), argumentList);
}

void DBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    // 导出属性变化的信号
    if (message.arguments().size() < 2)
        return;

    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
