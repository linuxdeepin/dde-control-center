/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COMPLEXITEM_H
#define COMPLEXITEM_H

#include <QObject>
#include <QWidget>

#include "dbus/dbusnetwork.h"
#include "dbus/dbusbluetooth.h"

class ComplexItem : public QWidget
{
    Q_OBJECT
public:
    explicit ComplexItem(DBusNetwork *dbusNetwork, QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);

private:
    void drawBackground();
    void drawVpn();
    void drawMobile();
    void drawBluetooth();
    void drawWireless();
    void manuallyUpdate();

private:
    DBusBluetooth *m_dbusBluetooth;
    DBusNetwork *m_dbusNetwork;
    QImage m_cacheImg;
    bool m_manuallyUpdate;
    QString m_backgroundImg;
    QString m_vpnImg;
    QString m_wirelessImg;
    QString m_bluetoothImg;
};

#endif // COMPLEXITEM_H
