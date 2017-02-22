/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "bluetoothwidget.h"
#include "settingsgroup.h"

using namespace dcc;
using namespace dcc::bluetooth;
using namespace dcc::widgets;

BluetoothWidget::BluetoothWidget(BluetoothModel *model) :
    ModuleWidget(),
    m_model(model),
    m_mainGroup(new SettingsGroup)
{
    setObjectName("Bluetooth");
    setModel(model);
    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_mainGroup);

    QTimer::singleShot(1, this, &BluetoothWidget::setVisibleState);
}


void BluetoothWidget::setModel(BluetoothModel *model)
{
    model->disconnect(this);
    connect(model, &BluetoothModel::adapterAdded, this, &BluetoothWidget::addAdapter);
    connect(model, &BluetoothModel::adapterRemoved, this, &BluetoothWidget::removeAdapter);

    for (const Adapter *adapter : model->adapters())
        addAdapter(adapter);
}

void BluetoothWidget::addAdapter(const Adapter *adapter)
{
    if (!m_valueMap.contains(adapter)) {
        NextPageWidget *showBluetoothDetailWidget = new NextPageWidget;
        showBluetoothDetailWidget->setTitle(adapter->name());
        m_valueMap.insert(adapter, showBluetoothDetailWidget);
        m_mainGroup->appendItem(showBluetoothDetailWidget);

        connect(showBluetoothDetailWidget, &NextPageWidget::clicked, [=]{
            emit showBluetoothDetail(adapter);
        });

        connect(adapter, &Adapter::nameChanged, showBluetoothDetailWidget, &NextPageWidget::setTitle);

        setVisibleState();
    }
}

void BluetoothWidget::removeAdapter(const Adapter *adapter)
{
    if (m_valueMap.contains(adapter)) {
        m_mainGroup->removeItem(m_valueMap.value(adapter));
        m_valueMap.take(adapter)->deleteLater();
        Adapter *w = const_cast<Adapter*>(adapter);
        if (w) {
            w->deleteLater();
        }
        setVisibleState();
    }
}

void BluetoothWidget::setVisibleState()
{
    emit requestModuleVisible(m_valueMap.size());
}
