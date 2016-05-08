/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/


#include "wiredplugin.h"

#include <QLabel>
#include <QFile>
#include <QDBusConnection>

using namespace NetworkPlugin;

const QString WIRED_PLUGIN_ID = "wired_plugin_id";
WiredPlugin::WiredPlugin()
{
    m_dbusNetwork = new com::deepin::daemon::DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &WiredPlugin::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &WiredPlugin::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::StateChanged, this, &WiredPlugin::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &WiredPlugin::onConnectionsChanged);

    initSettings();
}

WiredPlugin::~WiredPlugin()
{

}

void WiredPlugin::init(DockPluginProxyInterface *proxy)
{
    m_proxy = proxy;
    m_mode = proxy->dockMode();
    m_wiredItem = nullptr;
    //for init
    if (m_mode != Dock::FashionMode) {
        onConnectionsChanged();
    }
}

QString WiredPlugin::getPluginName()
{
    return tr("Wired Network");
}

QStringList WiredPlugin::ids()
{
    if (m_mode != Dock::FashionMode && wirelessDevicesCount(m_dbusNetwork) == 0 && wiredDevicesCount(m_dbusNetwork) > 0) {
        return QStringList(WIRED_PLUGIN_ID);
    }
    else {
        return QStringList();
    }
}

QString WiredPlugin::getName(QString id)
{
    Q_UNUSED(id);

    return tr("Wired Network");
}

QString WiredPlugin::getTitle(QString id)
{
    Q_UNUSED(id)

    return wiredIsConnected(m_dbusNetwork) ? getWiredIp() : tr("Network Not Connected");
}

QString WiredPlugin::getCommand(QString)
{
    return "dde-control-center network";
}

QPixmap WiredPlugin::getIcon(QString)
{
    QPixmap icon(":/images/images/wire_on.png");
    return icon.scaled(Dock::APPLET_EFFICIENT_ICON_SIZE, Dock::APPLET_EFFICIENT_ICON_SIZE);
}

bool WiredPlugin::configurable(const QString &id)
{
    Q_UNUSED(id);

    return m_mode != Dock::FashionMode && wirelessDevicesCount(m_dbusNetwork) == 0;
}

bool WiredPlugin::enabled(const QString &id)
{
    return m_settings->value(settingEnabledKey(id), true).toBool();    //default enabled
}

void WiredPlugin::setEnabled(const QString &id, bool enabled)
{
    if (m_mode == Dock::FashionMode)
        return;

    m_settings->setValue(settingEnabledKey(id), enabled);

    onEnabledChanged(id);
}

QWidget * WiredPlugin::getApplet(QString id)
{
    Q_UNUSED(id)

    return NULL;
}

QWidget * WiredPlugin::getItem(QString id)
{
    if (m_mode == Dock::FashionMode)
        return NULL;
    else if (enabled(id)){
        if (m_wiredItem == nullptr)
            addNewItem(id);

        return m_wiredItem;
    }
    else
        return NULL;
}

void WiredPlugin::changeMode(Dock::DockMode newMode, Dock::DockMode oldMode)
{
    m_mode = newMode;
    if (m_dbusNetwork->isValid() && (newMode != oldMode)){
        if (newMode == Dock::FashionMode) {
            removeItem(WIRED_PLUGIN_ID);
        }
        else {
            onConnectionsChanged();
        }
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, WIRED_PLUGIN_ID);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, WIRED_PLUGIN_ID);
}

QString WiredPlugin::getMenuContent(QString)
{
    return "";
}

void WiredPlugin::invokeMenuItem(QString, QString, bool)
{

}

void WiredPlugin::initSettings()
{
    m_settings = new QSettings("deepin", "dde-dock-network-wired-plugin", this);
}

void WiredPlugin::addNewItem(const QString &id)
{
    if (m_wiredItem != nullptr)
        return;

    m_wiredItem = new QLabel;
    m_wiredItem->setFixedSize(Dock::APPLET_EFFICIENT_ITEM_WIDTH, Dock::APPLET_EFFICIENT_ITEM_HEIGHT);
    QString iconPath = wiredIsConnected(m_dbusNetwork) ? ":/images/images/wire_on.png" : ":/images/images/network-error.png";
    m_wiredItem->setPixmap(QPixmap(iconPath).scaled(m_wiredItem->size()));
    m_proxy->itemAddedEvent(id);

    //add setting line to dock plugins setting frame
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    //update setting state
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeTitle, id);
}

void WiredPlugin::removeItem(const QString &id)
{
    if (m_wiredItem != nullptr) {
        m_wiredItem->setVisible(false);
        m_proxy->itemRemovedEvent(id);
        m_wiredItem->deleteLater();
        m_wiredItem = nullptr;
        //remove setting line from dock plugins setting frame,should delete wireditem first
        m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeConfigurable, id);
    }
}

void WiredPlugin::onEnabledChanged(const QString &id)
{
    if (!m_proxy)
        return;

    removeItem(id);

    if (enabled(id)) {
        addNewItem(id);
    }

    m_proxy->infoChangedEvent(DockPluginInterface::InfoTypeEnable, id);
}

int retryTimes = 10;
void WiredPlugin::onConnectionsChanged()
{
    if (m_mode == Dock::FashionMode)
        return;

    if (!m_dbusNetwork->isValid() && retryTimes-- > 0) {
        QTimer *retryTimer = new QTimer;
        retryTimer->setSingleShot(true);
        connect(retryTimer, &QTimer::timeout, this, &WiredPlugin::onConnectionsChanged);
        connect(retryTimer, &QTimer::timeout, retryTimer, &QTimer::deleteLater);
        retryTimer->start(1000);
        qWarning() << "[WiredPlugin] Network dbus data is not ready!";
        return;
    }
    retryTimes = 10;

    if (wirelessDevicesCount(m_dbusNetwork) == 0 && wiredDevicesCount(m_dbusNetwork) > 0 && m_wiredItem == nullptr) {
        addNewItem(WIRED_PLUGIN_ID);
    }
    else if (!enabled(WIRED_PLUGIN_ID) || !configurable(WIRED_PLUGIN_ID)) {
        removeItem(WIRED_PLUGIN_ID);
    }

    // update wireditem status
    if (m_wiredItem)
    {
        QString iconPath = wiredIsConnected(m_dbusNetwork) ? ":/images/images/wire_on.png" : ":/images/images/network-error.png";
        m_wiredItem->setPixmap(QPixmap(iconPath).scaled(m_wiredItem->size()));
    }
}

QString WiredPlugin::getWiredIp()
{
    if (m_dbusNetwork->isValid()) {
        QList<NetworkPlugin::ActiveConnectionInfo> infoList = NetworkPlugin::getActiveConnectionsInfo(m_dbusNetwork);
        foreach (NetworkPlugin::ActiveConnectionInfo info, infoList) {
            if (info.connectionType == NetworkPlugin::ConnectionTypeWired)
                return info.ip4["Address"].toString();
        }

    }
    return QString();
}

QString WiredPlugin::settingEnabledKey(const QString &id)
{
    return QString::number(m_mode) + QString("/%1_enabled").arg(id);
}
