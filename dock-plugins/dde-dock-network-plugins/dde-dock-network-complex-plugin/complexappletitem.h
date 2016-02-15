/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef COMPLEXAPPLETITEM_H
#define COMPLEXAPPLETITEM_H

#include <QFrame>
#include <QLabel>
#include "dbus/dbusnetwork.h"
#include "dimagebutton.h"
#include "../network-data/networkdata.h"

DWIDGET_USE_NAMESPACE
using namespace NetworkPlugin;

class ComplexAppletItem final : public DImageButton
{
    Q_OBJECT
public:
    enum DeviceType {
        DeviceUnknow,
        DeviceVPN,
        DeviceBluetooth,
        DeviceWired,
        DeviceWireless,
        DeviceMobile
    };

    explicit ComplexAppletItem(QWidget *parent = 0);

    void setIcon(const QString &icon);
    QString index() const;
    void setIndex(const QString &value);

    DeviceType deviceType() const;
    void setDeviceType(const DeviceType &deviceType);

private:
    QLabel *m_indexLabel;
    ComplexAppletItem::DeviceType m_deviceType = DeviceUnknow;
};

#endif // COMPLEXAPPLETITEM_H
