/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"
#include "controlcenterproxy.h"

class BluetoothMainWidget;
class Bluetooth: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "bluetooth.json")
    Q_INTERFACES(ModuleInterface)

public:
    Bluetooth();
    ~Bluetooth() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

protected:
    void setProxy(ControlCenterProxyInterface *proxy) Q_DECL_OVERRIDE;

private:
    BluetoothMainWidget * m_mainWidget;
};

#endif
