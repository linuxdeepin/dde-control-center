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

#include "dockdbusproxy.h"

#include <QApplication>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QMetaObject>
#include <QTimer>

const static QString DaemonDockService = "org.deepin.dde.daemon.Dock1";
const static QString DaemonDockPath = "/org/deepin/dde/daemon/Dock1";
const static QString DaemonDockInterface = "org.deepin.dde.daemon.Dock1";
const static QString DockService = "org.deepin.dde.Dock1";
const static QString DockPath = "/org/deepin/dde/Dock1";
const static QString DockInterface = "org.deepin.dde.Dock1";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

QDBusArgument &operator<<(QDBusArgument &arg, const DockItemInfo &info)
{
    arg.beginStructure();
    arg << info.name << info.displayName << info.itemKey << info.settingKey << info.iconLight
        << info.iconDark << info.visible;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DockItemInfo &info)
{
    arg.beginStructure();
    arg >> info.name >> info.displayName >> info.itemKey >> info.settingKey >> info.iconLight
            >> info.iconDark >> info.visible;
    arg.endStructure();
    return arg;
}

DockDBusProxy::DockDBusProxy(QObject *parent)
    : QObject(parent)
    , m_daemonDockInter(new QDBusInterface(DaemonDockService,
                                           DaemonDockPath,
                                           DaemonDockInterface,
                                           QDBusConnection::sessionBus(),
                                           this))
    , m_dockInter(new QDBusInterface(
              DockService, DockPath, DockInterface, QDBusConnection::sessionBus(), this))
{
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "DisplayModeChanged",
                                          this,
                                          SIGNAL(DisplayModeChanged(int)));
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "PositionChanged",
                                          this,
                                          SIGNAL(PositionChanged(int)));
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "HideModeChanged",
                                          this,
                                          SIGNAL(HideModeChanged(int)));
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "WindowSizeEfficientChanged",
                                          this,
                                          SIGNAL(WindowSizeEfficientChanged(uint)));
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "WindowSizeFashionChanged",
                                          this,
                                          SIGNAL(WindowSizeFashionChanged(uint)));
    QDBusConnection::sessionBus().connect(DaemonDockService,
                                          DaemonDockPath,
                                          DaemonDockInterface,
                                          "showRecentChanged",
                                          this,
                                          SIGNAL(showRecentChanged(bool)));

    QDBusConnection::sessionBus().connect(DockService,
                                          DockPath,
                                          DockInterface,
                                          "showInPrimaryChanged",
                                          this,
                                          SLOT(ShowInPrimaryChanged(bool)));
    QDBusConnection::sessionBus().connect(DockService,
                                          DockPath,
                                          DockInterface,
                                          "pluginVisibleChanged",
                                          this,
                                          SLOT(pluginVisibleChanged(const QString &, bool)));

    regiestDockItemType();
}

int DockDBusProxy::displayMode()
{
    return qvariant_cast<int>(m_daemonDockInter->property("DisplayMode"));
}

void DockDBusProxy::setDisplayMode(int mode)
{
    m_daemonDockInter->setProperty("DisplayMode", QVariant::fromValue(mode));
}

int DockDBusProxy::position()
{
    return qvariant_cast<int>(m_daemonDockInter->property("Position"));
}

void DockDBusProxy::setPosition(int value)
{
    m_daemonDockInter->setProperty("Position", QVariant::fromValue(value));
}

int DockDBusProxy::hideMode()
{
    return qvariant_cast<int>(m_daemonDockInter->property("HideMode"));
}

void DockDBusProxy::setHideMode(int value)
{
    m_daemonDockInter->setProperty("HideMode", QVariant::fromValue(value));
}

uint DockDBusProxy::windowSizeEfficient()
{
    return qvariant_cast<uint>(m_daemonDockInter->property("WindowSizeEfficient"));
}

void DockDBusProxy::setWindowSizeEfficient(uint value)
{
    m_daemonDockInter->setProperty("WindowSizeEfficient", QVariant::fromValue(value));
}

uint DockDBusProxy::windowSizeFashion()
{
    return qvariant_cast<uint>(m_daemonDockInter->property("WindowSizeFashion"));
}

void DockDBusProxy::setWindowSizeFashion(uint value)
{
    m_daemonDockInter->setProperty("WindowSizeFashion", QVariant::fromValue(value));
}

bool DockDBusProxy::showInPrimary()
{
    return qvariant_cast<bool>(m_dockInter->property("showInPrimary"));
}

void DockDBusProxy::setShowInPrimary(bool value)
{
    m_dockInter->setProperty("showInPrimary", QVariant::fromValue(value));
}

bool DockDBusProxy::showRecent()
{
    return qvariant_cast<bool>(m_daemonDockInter->property("ShowRecent"));
}

void DockDBusProxy::regiestDockItemType()
{
    static bool isRegister = false;
    if (isRegister)
        return;

    qRegisterMetaType<DockItemInfo>("DockItemInfo");
    qDBusRegisterMetaType<DockItemInfo>();
    qRegisterMetaType<DockItemInfos>("DockItemInfos");
    qDBusRegisterMetaType<DockItemInfos>();
    isRegister = true;
}

void DockDBusProxy::resizeDock(int offset, bool dragging)
{
    m_dockInter->call(QDBus::CallMode::Block,
                      QStringLiteral("resizeDock"),
                      QVariant::fromValue(offset),
                      QVariant::fromValue(dragging));
}

QDBusPendingReply<QStringList> DockDBusProxy::GetLoadedPlugins()
{
    QDBusPendingReply<QStringList> reply =
            m_dockInter->asyncCall(QStringLiteral("GetLoadedPlugins"));
    reply.waitForFinished();
    return reply;
}

QDBusPendingReply<QString> DockDBusProxy::getPluginKey(const QString &pluginName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("getPluginKey"), argumentList);
}

QDBusPendingReply<bool> DockDBusProxy::getPluginVisible(const QString &pluginName)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("getPluginVisible"), argumentList);
}

QDBusPendingReply<> DockDBusProxy::setPluginVisible(const QString &pluginName, bool visible)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(pluginName) << QVariant::fromValue(visible);
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("setPluginVisible"), argumentList);
}

QDBusPendingReply<> DockDBusProxy::SetShowRecent(bool visible)
{
    QList<QVariant> argumengList;
    argumengList << QVariant::fromValue(visible);
    return m_daemonDockInter->asyncCallWithArgumentList(QStringLiteral("SetShowRecent"),
                                                        argumengList);
}

QDBusPendingReply<DockItemInfos> DockDBusProxy::plugins()
{
    QDBusPendingReply<DockItemInfos> reply = m_dockInter->asyncCall(QStringLiteral("plugins"));
    reply.waitForFinished();
    return reply;
}

QDBusPendingReply<> DockDBusProxy::setItemOnDock(const QString settingKey,
                                                 const QString &itemKey,
                                                 bool visible)
{
    QList<QVariant> argumengList;
    argumengList << settingKey << itemKey << QVariant::fromValue(visible);
    return m_dockInter->asyncCallWithArgumentList("setItemOnDock", argumengList);
}
