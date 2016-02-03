/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDBusPendingReply>
#include <QFile>
#include "complexapplet.h"
#include "../network-data/networkdata.h"

using namespace NetworkPlugin;

const int MAX_COLUMN_COUNT = 4;
const int NORMAL_COLUMN_COUNT = 3;
const int ICON_SIZE = 40;
const QString VPN_CONNECTION_ID = "vpn_connection_id";

ComplexApplet::ComplexApplet(QWidget *parent)
    : QFrame(parent)
{
    initStyleSheet();

    m_dbusNetwork = new DBusNetwork(this);
    connect(m_dbusNetwork, &DBusNetwork::VpnEnabledChanged, this, &ComplexApplet::onVpnEnableChanged);
    connect(m_dbusNetwork, &DBusNetwork::ConnectionsChanged, this, &ComplexApplet::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::ActiveConnectionsChanged, this, &ComplexApplet::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::DevicesChanged, this, &ComplexApplet::onConnectionsChanged);
    connect(m_dbusNetwork, &DBusNetwork::DeviceEnabled, this, &ComplexApplet::onDeviceEnabledChanged);

    m_dbusBluetooth = new DBusBluetooth(this);
    connect(m_dbusBluetooth, &DBusBluetooth::AdapterRemoved, this, &ComplexApplet::onBluetoothAdapterRemoved);
    connect(m_dbusBluetooth, &DBusBluetooth::AdapterPropertiesChanged, this, &ComplexApplet::onBluetoothAdaptersChanged);

    m_mainLayout = new QGridLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    //for init
    onConnectionsChanged();
    onBluetoothAdaptersChanged();
}

ComplexApplet::~ComplexApplet()
{

}

int ComplexApplet::itemCount() const
{
    return m_itemMap.count();
}

void ComplexApplet::relayout()
{
    clearLayout();
    int itemCount = m_itemMap.count();
    if (itemCount > 0) {
        int maxColumn = NORMAL_COLUMN_COUNT;
        //min column should be 2 and the max should be 4
        if (itemCount % NORMAL_COLUMN_COUNT == 1)
            maxColumn = MAX_COLUMN_COUNT;

        int row = 1;
        int column = 0;
        for (ComplexAppletItem *item : m_itemMap.values()) {
            m_mainLayout->addWidget(item, row, column, Qt::AlignCenter);
            if (++column >= maxColumn) {
                column = 0;
                row ++;
            }
        }

        if (itemCount <= maxColumn)
            setFixedSize(itemCount * ICON_SIZE, ICON_SIZE);
        else
            setFixedSize(maxColumn * ICON_SIZE, row * ICON_SIZE);
    }

    emit sizeChanged();
}

void ComplexApplet::clearLayout()
{
    while(m_mainLayout->count() > 0){
        QLayoutItem *item = m_mainLayout->takeAt(0);
        delete item;
    }
}

void ComplexApplet::initStyleSheet()
{
    QFile file(":/qss/qss/complex.qss");
    if (file.open(QFile::ReadOnly))
    {
        QString styleSheet = QString(file.readAll());
        setStyleSheet(styleSheet);
        file.close();
    }
    else {
        qWarning() << "[ComplexNetworkPlugin] Open  style file errr!";
    }
}

void ComplexApplet::onBluetoothAdaptersChanged()
{
    QList<BluetoothAdapterInfo> list = bluetoothAdapters(m_dbusBluetooth);
    int listCount = list.count();
    for (int i = 0; i < listCount; i ++) {
        BluetoothAdapterInfo info = list.at(i);
        if (m_itemMap.keys().indexOf(info.path) == -1) {
            onBluetoothAdapterAdded(info.path);
        }
        updateBluetoothIcon(info.path, info.powered, listCount > 1 ? QString::number(i + 1) : "");
    }
}

void ComplexApplet::onConnectionsChanged()
{
    handleVpnConnections();
    handleWiredConnections();
    handleMobileConnections();
    handleWirelessConnections();
}

void ComplexApplet::onVpnEnableChanged()
{
    updateVpnIcon();
}

void ComplexApplet::onDeviceEnabledChanged(const QString &path, bool enabled)
{
    updateWirelessIcon(path, enabled);
}

void ComplexApplet::handleWiredConnections()
{
    QStringList pathList;
    QJsonArray pathArray = deviceArray(ConnectionTypeWired, m_dbusNetwork);
    for (QJsonValue pathValue : pathArray) {
        pathList << pathValue.toObject().value("Path").toString();
    }
    //add new and update icon
    for (int i = 0; i < pathList.length(); i ++) {
        QString path = pathList.at(i);
        if (m_wiredList.indexOf(path) == -1) {
            ComplexAppletItem *item = new ComplexAppletItem(this);
            item->setDeviceType(ComplexAppletItem::DeviceWired);
            connect(item, &ComplexAppletItem::clicked, [=] {
                bool enabled = m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(path));
                m_dbusNetwork->EnableDevice(QDBusObjectPath(path), !enabled);
            });
            m_wiredList << path;
            m_itemMap.insert(path, item);
            relayout();
        }

        updateWiredIcon(path,
                        m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(path)),
                        pathList.length() > 1 ? QString::number(i + 1) : "");
    }
    //remove old
    for (QString path : m_wiredList) {
        if (pathList.indexOf(path) == -1) {
            m_wiredList.removeAll(path);
            m_itemMap.take(path)->deleteLater();
            relayout();
        }
    }
}

void ComplexApplet::handleWirelessConnections()
{
    QStringList pathList;
    QJsonArray pathArray = deviceArray(ConnectionTypeWireless, m_dbusNetwork);
    for (QJsonValue pathValue : pathArray) {
        pathList << pathValue.toObject().value("Path").toString();
    }
    //add new and update icon
    for (int i = 0; i < pathList.length(); i ++) {
        QString path = pathList.at(i);
        if (m_wirelessList.indexOf(path) == -1) {
            ComplexAppletItem *item = new ComplexAppletItem(this);
            item->setDeviceType(ComplexAppletItem::DeviceWireless);
            connect(item, &ComplexAppletItem::clicked, [=] {
                bool enabled = m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(path));
                m_dbusNetwork->EnableDevice(QDBusObjectPath(path), !enabled);
            });
            m_wirelessList << path;
            m_itemMap.insert(path, item);
            relayout();
        }

        updateWirelessIcon(path,
                           m_dbusNetwork->IsDeviceEnabled(QDBusObjectPath(path)),
                           pathList.length() > 1 ? QString::number(i + 1) : "");
    }
    //remove old
    for (QString path : m_wirelessList) {
        if (pathList.indexOf(path) == -1) {
            m_wirelessList.removeAll(path);
            m_itemMap.take(path)->deleteLater();
            relayout();
        }
    }
}

void ComplexApplet::handleMobileConnections()
{
    //TODO
}

void ComplexApplet::handleVpnConnections()
{
    if (hasVpn(m_dbusNetwork)) {
        if (m_itemMap.keys().indexOf(VPN_CONNECTION_ID) == -1) {
            ComplexAppletItem *item = new ComplexAppletItem(this);
            item->setDeviceType(ComplexAppletItem::DeviceVPN);
            connect(item, &ComplexAppletItem::clicked, [=] {
                m_dbusNetwork->setVpnEnabled(!m_dbusNetwork->vpnEnabled());
            });
            m_itemMap.insert(VPN_CONNECTION_ID, item);
            relayout();
        }
        updateVpnIcon();
    }
    else if (m_itemMap.keys().indexOf(VPN_CONNECTION_ID) != -1) {
        m_itemMap.take(VPN_CONNECTION_ID)->deleteLater();
        relayout();
    }
}

void ComplexApplet::onBluetoothAdapterAdded(const QString &path)
{
    if (m_itemMap.keys().indexOf(path) == -1) {
        ComplexAppletItem *item = new ComplexAppletItem(this);
        connect(item, &ComplexAppletItem::clicked, [=]{
            QList<BluetoothAdapterInfo> list = bluetoothAdapters(m_dbusBluetooth);
            for (BluetoothAdapterInfo info : list) {
                if (info.path == path){
                    m_dbusBluetooth->SetAdapterPowered(QDBusObjectPath(path), !info.powered);
                    break;
                }
            }
        });
        m_itemMap.insert(path, item);
        relayout();
    }
}

void ComplexApplet::onBluetoothAdapterRemoved(const QString &info)
{
    QJsonObject obj = QJsonDocument::fromJson(info.toUtf8()).object();
    QString path = obj["Path"].toString();
    if (m_itemMap.keys().indexOf(path) != -1) {
        m_itemMap.take(path)->deleteLater();
        relayout();
    }
}

void ComplexApplet::updateVpnIcon()
{
    if (m_itemMap.value(VPN_CONNECTION_ID)) {
        QString img = m_dbusNetwork->vpnEnabled()
                ? ":/images/images/switch_vpn_on.png"
                : ":/images/images/switch_vpn_off.png";
        m_itemMap.value(VPN_CONNECTION_ID)->setIcon(img);
    }
}

void ComplexApplet::updateWiredIcon(const QString &path, bool connected, const QString &index)
{
    ComplexAppletItem *item = m_itemMap.value(path);
    if (item && item->deviceType() == ComplexAppletItem::DeviceWired) {
        QString img = connected
                ? ":/images/images/switch_wired_on.png"
                : ":/images/images/switch_wired_off.png";
        item->setIcon(img);
        item->setIndex(index == "default" ? item->index() : index);
    }
}

void ComplexApplet::updateBluetoothIcon(const QString &path, bool powered, const QString &index)
{
    ComplexAppletItem *item = m_itemMap.value(path);
    if (item) {
        QString img = powered
                ? ":/images/images/switch_bluetooth_on.png"
                : ":/images/images/switch_bluetooth_off.png";
        item->setIcon(img);
        item->setIndex(index);
    }
}

void ComplexApplet::updateWirelessIcon(const QString &path, bool connected, const QString &index)
{
    ComplexAppletItem *item = m_itemMap.value(path);
    if (item && item->deviceType() == ComplexAppletItem::DeviceWireless) {
        QString img = connected
                ? ":/images/images/switch_wireless_on.png"
                : ":/images/images/switch_wireless_off.png";
        item->setIcon(img);
        item->setIndex(index == "default" ? item->index() : index);
    }
}
