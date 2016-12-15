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
    setObjectName("Bluetooth");

    setTitle(tr("Bluetooth"));

    setModel(model);
}

void BluetoothWidget::setModel(BluetoothModel *model)
{
    model->disconnect(this);
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemoved, this, &BluetoothWidget::removeAdapter);

    QStringList tmpList;
    for (const Adapter *adapter : model->adapters()) {
        const QString adapterId = adapter->id();
        tmpList << adapterId;

        if (!widgetByAdapterId(adapterId)) {
            addAdapter(adapter);
        }
    }

    QList<AdapterWidget*> aws = findChildren<AdapterWidget*>();
    for (AdapterWidget *aw : aws) {
        if (!tmpList.contains(aw->adapter()->id())) {
            m_centeralLayout->removeWidget(aw);
            aw->deleteLater();
        }
    }

    setVisible(model->adapters().length() != 0);
}

AdapterWidget *BluetoothWidget::widgetByAdapterId(const QString &adapterId)
{
    QList<AdapterWidget*> aws = findChildren<AdapterWidget*>();
    for (AdapterWidget *aw : aws) {
        if (aw->adapter()->id() == adapterId) {
            return aw;
        }
    }

    return nullptr;
}

void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    if (widgetByAdapterId(adapter->id())) return;

    AdapterWidget * widget = new AdapterWidget(adapter);
    m_widgets.append(widget);

    m_centeralLayout->addWidget(widget);

    connect(widget, &AdapterWidget::requestToggleAdapter, [this, adapter] (const bool &toggled) { emit requestToggleAdapter(adapter, toggled); });
    connect(widget, &AdapterWidget::requestConnectDevice, this, &BluetoothWidget::requestConnectDevice);
    connect(widget, &AdapterWidget::requestShowDetail, this, &BluetoothWidget::requestShowDetail);

    setVisible(true);
}

void BluetoothWidget::removeAdapter(const QString &adapterId)
{
    AdapterWidget *aw = widgetByAdapterId(adapterId);
    if (aw) {
        m_centeralLayout->removeWidget(aw);
        aw->deleteLater();
    }

    if (m_model->adapters().length() == 0) {
        setVisible(false);
    }
}

} // namespace bluetooth
} // namespace dcc
