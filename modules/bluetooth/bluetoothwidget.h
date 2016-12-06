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

signals:
    void requestToggleAdapter(const Adapter *adapter, const bool &toggled);

private slots:
    void addAdapter(const Adapter *adapter);

private:
    BluetoothModel *m_model;
    QList<AdapterWidget *> m_widgets;
};

} // namespace bluetooth
} // namespace dcc

#endif // DCC_BLUETOOTH_BLUETOOTHWIDGET_H
