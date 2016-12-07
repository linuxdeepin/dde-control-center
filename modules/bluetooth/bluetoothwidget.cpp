/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "bluetoothwidget.h"

namespace dcc {
namespace bluetooth {

BluetoothWidget::BluetoothWidget(BluetoothModel *model) :
    ModuleWidget(),
    m_model(model)
{
    setTitle(tr("Bluetooth"));

    setModel(model);
}

void BluetoothWidget::setModel(BluetoothModel *model)
{
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemoved, this, &BluetoothWidget::removeAdapter);

    for (const Adapter *adapter : model->adapters()) {
        addAdapter(adapter);
    }
}

void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    AdapterWidget * widget = new AdapterWidget(adapter);
    m_widgets.append(widget);

    m_centeralLayout->addWidget(widget);

    connect(widget, &AdapterWidget::requestToggleAdapter, [this, adapter] (const bool &toggled) { emit requestToggleAdapter(adapter, toggled); });
    connect(widget, &AdapterWidget::requestConnectDevice, this, &BluetoothWidget::requestConnectDevice);
    connect(widget, &AdapterWidget::requestShowDetail, this, &BluetoothWidget::requestShowDetail);
}

void BluetoothWidget::removeAdapter(const QString &adapterId)
{
    QList<AdapterWidget*> aws = findChildren<AdapterWidget*>();
    for (AdapterWidget *aw : aws) {
        if (aw->adapter()->id() == adapterId) {
            m_centeralLayout->removeWidget(aw);
            aw->deleteLater();
        }
    }
}

} // namespace bluetooth
} // namespace dcc
