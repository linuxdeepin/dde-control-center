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
#include "translucentframe.h"
#include "settingsheaderitem.h"
#include "loadingindicator.h"

namespace dcc {
namespace bluetooth {

AdapterWidget::AdapterWidget(const Adapter *adapter) :
    m_adapter(adapter),
    m_titleEdit(new TitleEdit),
    m_switch(new SwitchWidget(m_titleEdit)),
    m_titleGroup(new SettingsGroup),
    m_myDevicesGroup(new SettingsGroup(tr("My devices"))),
    m_otherDevicesGroup(new SettingsGroup(tr("Other devices")))
{
    setTitle(adapter->name());

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    layout->setSpacing(10);

    m_titleGroup->appendItem(m_switch);
    m_myDevicesGroup->setHeaderVisible(true);
    m_otherDevicesGroup->setHeaderVisible(true);

    LoadingIndicator *load = new LoadingIndicator;
    load->setTheme("bluetooth");
    m_otherDevicesGroup->headerItem()->setRightWidget(load);
    load->play();

    m_tip = new QLabel(tr("Enable bluetooth to find nearby devices (loudspeaker, keyboard, mouse)"));
    m_tip->setVisible(!m_switch->checked());
    m_tip->setWordWrap(true);

    layout->addSpacing(10);
    layout->addWidget(m_titleGroup);
    layout->addWidget(m_tip, 0, Qt::AlignTop);
    layout->addWidget(m_myDevicesGroup);
    layout->addWidget(m_otherDevicesGroup);
    layout->addStretch();

    connect(m_switch, &SwitchWidget::checkedChanged, this, &AdapterWidget::toggleSwitch);
    connect(m_switch, &SwitchWidget::checkedChanged, [=](bool state) {
        m_tip->setVisible(!state);
    });
    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, [=](const QString &alias) {
        emit requestSetAlias(adapter, alias);
    });

    connect(adapter, &Adapter::nameChanged, this, &AdapterWidget::setTitle);
    connect(adapter, &Adapter::destroyed, this, &AdapterWidget::back);

    TranslucentFrame *w = new TranslucentFrame;
    w->setLayout(layout);
    setContent(w);

    setAdapter(adapter);
}

void AdapterWidget::setAdapter(const Adapter *adapter)
{
    connect(adapter, &Adapter::nameChanged, m_titleEdit, &TitleEdit::setTitle);
    connect(adapter, &Adapter::deviceAdded, this, &AdapterWidget::addDevice);
    connect(adapter, &Adapter::deviceRemoved, this, &AdapterWidget::removeDevice);
    connect(adapter, &Adapter::poweredChanged, m_switch, &SwitchWidget::setChecked);

    m_switch->blockSignals(true);
    m_titleEdit->setTitle(adapter->name());
    m_switch->setChecked(adapter->powered());
    m_tip->setVisible(!m_switch->checked());
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

    emit requestSetToggleAdapter(m_adapter, checked);
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
    connect(w, &DeviceSettingsItem::requestShowDetail, [this] (const Device *device) {
        emit requestShowDetail(m_adapter, device);
    });
}

void AdapterWidget::removeDevice(const QString &deviceId)
{
    QList<DeviceSettingsItem*> devices = m_myDevicesGroup->findChildren<DeviceSettingsItem*>();
    for (DeviceSettingsItem *item : devices) {
        if (item->device()->id() == deviceId) {
            m_myDevicesGroup->removeItem(item);
            item->deleteLater();
            return;
        }
    }

    devices = m_otherDevicesGroup->findChildren<DeviceSettingsItem*>();
    for (DeviceSettingsItem *item : devices) {
        if (item->device()->id() == deviceId) {
            m_otherDevicesGroup->removeItem(item);
            item->deleteLater();
            return;
        }
    }
}

const Adapter *AdapterWidget::adapter() const
{
    return m_adapter;
}

}
}
