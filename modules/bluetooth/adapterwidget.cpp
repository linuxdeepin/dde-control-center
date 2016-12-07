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

#include "devicesettingsitem.h"

namespace dcc {
namespace bluetooth {

AdapterWidget::AdapterWidget(const Adapter *adapter) :
    QWidget(),
    m_switch(new SwitchWidget),
    m_titleGroup(new SettingsGroup),
    m_myDevicesGroup(new SettingsGroup(tr("My devices"))),
    m_otherDevicesGroup(new SettingsGroup(tr("Other devices")))
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_titleGroup->appendItem(m_switch);
    m_myDevicesGroup->setHeaderVisible(true);
    m_otherDevicesGroup->setHeaderVisible(true);

    layout->addWidget(m_titleGroup);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_otherDevicesGroup);

    connect(m_switch, &SwitchWidget::checkedChanegd, this, &AdapterWidget::toggleSwitch);
    setAdapter(adapter);
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_switch, &SwitchWidget::setTitle);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice);
    connect(adapter, &Adapter::poweredChanged, m_switch, &SwitchWidget::setChecked);

    m_switch->blockSignals(true);
    m_switch->setTitle(adapter->name());
    m_switch->setChecked(adapter->powered());
    m_switch->blockSignals(false);

    blockSignals(true);
    toggleSwitch(adapter->powered());
    blockSignals(false);

    for (const Device *device : adapter->devices()) {
        addDevice(device);
    }
}

void AdapterWidget::toggleSwitch(const bool &checked)
{
    m_myDevicesGroup->setVisible(checked);
    m_otherDevicesGroup->setVisible(checked);

    emit requestToggleAdapter(checked);
}

void AdapterWidget::addDevice(const Device *device)
{

    DeviceSettingsItem *w = new DeviceSettingsItem(device);

    auto CategoryDevice = [this, w] (const bool paired) {
        if (paired) {
            m_myDevicesGroup->appendItem(w);
        } else {
            m_otherDevicesGroup->appendItem(w);
        }
    };
    CategoryDevice(device->paired());

    connect(w, &DeviceSettingsItem::requestConnectDevice, this, &AdapterWidget::requestConnectDevice);
    connect(device, &Device::pairedChanged, CategoryDevice);
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    Q_UNUSED(deviceId);
}

}
}
