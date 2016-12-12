/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_BLUETOOTH_BLUETOOTHWIDGET_H
#define DCC_BLUETOOTH_BLUETOOTHWIDGET_H

#include "modulewidget.h"
#include "bluetoothmodel.h"

#include "adapterwidget.h"

namespace dcc {
namespace bluetooth {

class BluetoothWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit BluetoothWidget(BluetoothModel *model = 0);

    void setModel(BluetoothModel *model);
    AdapterWidget *widgetByAdapterId(const QString &adapterId);

signals:
    void requestToggleAdapter(const Adapter *adapter, const bool &toggled);
    void requestConnectDevice(const Device *device);
    void requestShowDetail(const Adapter *adapter, const Device *device);

private slots:
    void addAdapter(const Adapter *adapter);
    void removeAdapter(const QString &adapterId);

private:
    BluetoothModel *m_model;
    QList<AdapterWidget *> m_widgets;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWIDGET_H
