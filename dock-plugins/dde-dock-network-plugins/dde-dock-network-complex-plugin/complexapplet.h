/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COMPLEXAPPLET_H
#define COMPLEXAPPLET_H

#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include "complexappletitem.h"
#include "dbus/dbusnetwork.h"
#include "dbus/dbusbluetooth.h"

DUI_USE_NAMESPACE

class ComplexApplet : public QFrame
{
    Q_OBJECT
public:
    explicit ComplexApplet(QWidget *parent = 0);
    ~ComplexApplet();
    int itemCount() const;

signals:
    void sizeChanged();

private:
    void relayout();
    void clearLayout();
    void initStyleSheet();
    void onConnectionsChanged();
    void onVpnEnableChanged();
    void onDeviceEnabledChanged(const QString &path, bool enabled);
    void onBluetoothAdapterAdded(const QString &path);
    void onBluetoothAdapterRemoved(const QString &info);
    void onBluetoothAdaptersChanged();
    void handleWiredConnections();
    void handleWirelessConnections();
    void handleMobileConnections();
    void handleVpnConnections();
    void updateVpnIcon();
    void updateWiredIcon(const QString &path, bool connected, const QString &index = "default");
    void updateBluetoothIcon(const QString &path, bool powered, const QString &index = "");
    void updateWirelessIcon(const QString &path, bool connected, const QString &index = "default");

private:
    QGridLayout *m_mainLayout;
    DBusNetwork *m_dbusNetwork;
    DBusBluetooth *m_dbusBluetooth;
    QStringList m_wiredList;
    QStringList m_wirelessList;
    QMap<QString, ComplexAppletItem *> m_itemMap;
};

#endif // COMPLEXAPPLET_H
