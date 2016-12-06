/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "adapterwidget.h"

#include <QVBoxLayout>
#include <QDebug>

#include "nextpagewidget.h"

namespace dcc {
namespace bluetooth {

AdapterWidget::AdapterWidget(const Adapter *adapter) :
    QWidget(),
    m_switch(new SwitchWidget),
    m_myDevicesGroup(new SettingsGroup(tr("My devices"))),
    m_otherDevicesGroup(new SettingsGroup(tr("Other devices")))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_myDevicesGroup->setHeaderVisible(true);
    m_otherDevicesGroup->setHeaderVisible(true);

    layout->addWidget(m_switch);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_otherDevicesGroup);

    setAdapter(adapter);
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_switch, &SwitchWidget::setTitle);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice);

    m_switch->setTitle(adapter->name());
    for (const Device *device : adapter->devices()) {
        addDevice(device);
    }
}

void AdapterWidget::addDevice(const Device *device)
{
    NextPageWidget *w = new NextPageWidget;

    connect(device, &Device::nameChanged, w, &NextPageWidget::setTitle);
    w->setTitle(device->name());

    if (device->paired()) {
        m_myDevicesGroup->appendItem(w);
    } else {
        m_otherDevicesGroup->appendItem(w);
    }
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    Q_UNUSED(deviceId);
}

}
}
