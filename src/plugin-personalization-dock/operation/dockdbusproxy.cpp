//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "dockdbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QTimer>
#include <QApplication>

const static QString DockService = "org.deepin.dde.Dock1";
const static QString DockPath = "/org/deepin/dde/Dock1";
const static QString DockInterface = "org.deepin.dde.Dock1";

const static QString PropertiesInterface = "org.freedesktop.DBus.Properties";
const static QString PropertiesChanged = "PropertiesChanged";

QDBusArgument &operator<<(QDBusArgument &arg, const DockItemInfo &info)
{
    arg.beginStructure();
    arg << info.name << info.displayName << info.itemKey << info.settingKey << info.iconLight << info.iconDark << info.visible;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, DockItemInfo &info)
{
    arg.beginStructure();
    arg >> info.name >> info.displayName >> info.itemKey >> info.settingKey >> info.iconLight >> info.iconDark >> info.visible;
    arg.endStructure();
    return arg;
}

DockDBusProxy::DockDBusProxy(QObject *parent)
    : QObject(parent)
    , m_dockInter(new QDBusInterface(DockService, DockPath, DockInterface, QDBusConnection::sessionBus(), this))
{
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "DisplayModeChanged", this, SIGNAL(DisplayModeChanged(int)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "PositionChanged", this, SIGNAL(PositionChanged(int)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "HideModeChanged", this, SIGNAL(HideModeChanged(int)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "WindowSizeEfficientChanged", this, SIGNAL(WindowSizeEfficientChanged(uint)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "WindowSizeFashionChanged", this, SIGNAL(WindowSizeFashionChanged(uint)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "showRecentChanged", this, SIGNAL(showRecentChanged(bool)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "showInPrimaryChanged", this, SLOT(ShowInPrimaryChanged(bool)));
    QDBusConnection::sessionBus().connect(DockService, DockPath, DockInterface, "pluginVisibleChanged", this, SLOT(pluginVisibleChanged(const QString &, bool)));

    regiestDockItemType();
}

int DockDBusProxy::displayMode()
{
    return qvariant_cast<int>(m_dockInter->property("DisplayMode"));
}

void DockDBusProxy::setDisplayMode(int mode)
{
    m_dockInter->setProperty("DisplayMode", QVariant::fromValue(mode));
}

int DockDBusProxy::position()
{
    return qvariant_cast<int>(m_dockInter->property("Position"));
}

void DockDBusProxy::setPosition(int value)
{
    m_dockInter->setProperty("Position", QVariant::fromValue(value));
}

int DockDBusProxy::hideMode()
{
    return qvariant_cast<int>(m_dockInter->property("HideMode"));
}

void DockDBusProxy::setHideMode(int value)
{
    m_dockInter->setProperty("HideMode", QVariant::fromValue(value));
}

uint DockDBusProxy::windowSizeEfficient()
{
    return qvariant_cast<uint>(m_dockInter->property("WindowSizeEfficient"));
}

void DockDBusProxy::setWindowSizeEfficient(uint value)
{
    m_dockInter->setProperty("WindowSizeEfficient", QVariant::fromValue(value));
}

uint DockDBusProxy::windowSizeFashion()
{
    return qvariant_cast<uint>(m_dockInter->property("WindowSizeFashion"));
}

void DockDBusProxy::setWindowSizeFashion(uint value)
{
    m_dockInter->setProperty("WindowSizeFashion", QVariant::fromValue(value));
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
    return qvariant_cast<bool>(m_dockInter->property("ShowRecent"));
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
    m_dockInter->call(QDBus::CallMode::Block, QStringLiteral("resizeDock"), QVariant::fromValue(offset), QVariant::fromValue(dragging));
}

QDBusPendingReply<QStringList> DockDBusProxy::GetLoadedPlugins()
{
    QDBusPendingReply<QStringList> reply = m_dockInter->asyncCall(QStringLiteral("GetLoadedPlugins"));
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
    return m_dockInter->asyncCallWithArgumentList(QStringLiteral("SetShowRecent"), argumengList);
}

QDBusPendingReply<DockItemInfos> DockDBusProxy::plugins()
{
    QDBusPendingReply<DockItemInfos> reply = m_dockInter->asyncCall(QStringLiteral("plugins"));
    reply.waitForFinished();
    return reply;
}

QDBusPendingReply<> DockDBusProxy::setItemOnDock(const QString settingKey, const QString &itemKey, bool visible)
{
    QList<QVariant> argumengList;
    argumengList << settingKey << itemKey << QVariant::fromValue(visible);
    return m_dockInter->asyncCallWithArgumentList("setItemOnDock", argumengList);
}
