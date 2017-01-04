/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "devicesettingsitem.h"

#include <QHBoxLayout>
#include <QMouseEvent>

#include "loadingindicator.h"
#include "labels/normallabel.h"
#include "nextbutton.h"

using namespace dcc::widgets;

namespace dcc {
namespace bluetooth {

DeviceSettingsItem::DeviceSettingsItem(const Device *device) :
    SettingsItem(),
    m_device(device),
    m_titleLabel(new NormalLabel),
    m_tipLabel(new NormalLabel),
    m_loadingIndicator(new LoadingIndicator),
    m_nextButton(new NextButton)
{
    setFixedHeight(36);

    m_titleLabel->setObjectName("DeviceTitle");
    m_tipLabel->setObjectName("DeviceState");

    m_loadingIndicator->setFixedSize(24, 24);
    m_loadingIndicator->hide();

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    layout->addSpacing(20);
    layout->addWidget(m_titleLabel);
    layout->addStretch();
    layout->addWidget(m_tipLabel);
    layout->addWidget(m_loadingIndicator);
    layout->addWidget(m_nextButton);
    layout->addSpacing(10);
    setLayout(layout);

    setDevice(device);

    connect(m_nextButton, &widgets::NextButton::clicked, [this] { emit requestShowDetail(m_device); });
}

void DeviceSettingsItem::setDevice(const Device *device)
{
    connect(device, &Device::nameChanged, m_titleLabel, &NormalLabel::setText);
    connect(device, &Device::stateChanged, this, &DeviceSettingsItem::onDeviceStateChanged);
    connect(device, &Device::pairedChanged, this, &DeviceSettingsItem::onDevicePairedChanged);
    connect(device, &Device::connectingChanged, this, &DeviceSettingsItem::setLoading);

    m_titleLabel->setText(device->name());
    setLoading(device->connecting());
    onDeviceStateChanged(device->state());
    onDevicePairedChanged(device->paired());
}

void DeviceSettingsItem::setLoading(const bool &loading)
{
    m_loadingIndicator->play();
    loading ? m_loadingIndicator->show() : m_loadingIndicator->hide();
    loading ? m_tipLabel->hide() : m_tipLabel->show();
}

void DeviceSettingsItem::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_device->state() != Device::StateConnected) {
        event->accept();

        emit requestConnectDevice(m_device);
    }

    SettingsItem::mouseReleaseEvent(event);
}

void DeviceSettingsItem::onDeviceStateChanged(const Device::State &state)
{
    qDebug() << "device state changed: " << m_device;

    const bool connected = state == Device::StateConnected;
    const QString tip = connected ? tr("Connected") : "";
    m_tipLabel->setText(tip);

    if (connected) setLoading(false);
}

void DeviceSettingsItem::onDevicePairedChanged(const bool &paired)
{
    if (paired) {
        m_nextButton->show();
        m_tipLabel->show();
    } else {
        m_nextButton->hide();
        m_tipLabel->hide();
    }
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}

} // namespace bluetooth
} // namespace dcc
