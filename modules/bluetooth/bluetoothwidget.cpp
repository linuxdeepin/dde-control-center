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
}

} // namespace bluetooth
} // namespace dcc
