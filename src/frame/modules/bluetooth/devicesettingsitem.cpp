/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devicesettingsitem.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>

#include "widgets/loadingindicator.h"
#include "widgets/labels/normallabel.h"
#include "widgets/nextbutton.h"

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

    m_loadingIndicator->setTheme("dark");
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
    layout->addSpacing(4);
    layout->addWidget(m_nextButton);
    layout->addSpacing(10);
    setLayout(layout);

    setDevice(device);

    connect(m_nextButton, &widgets::NextButton::clicked, [this] { Q_EMIT requestShowDetail(m_device); });
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

        Q_EMIT requestConnectDevice(m_device);
    }

    if (m_device->state() == Device::StateConnected) {
        setLoading(true);
        QTimer::singleShot(1000, this, &DeviceSettingsItem::onStopLoadingAnimation);
    }

    SettingsItem::mouseReleaseEvent(event);
}

void DeviceSettingsItem::onDeviceStateChanged(const Device::State &state)
{
    qDebug() << "device state changed: " << m_device;

    if (state == Device::StateAvailable)
        setLoading(true);

    QString tip;

    switch(state) {
    case Device::StateConnected:
    {
        tip = tr("Connected");
        setLoading(false);
        break;
    }
    case Device::StateUnavailable:
        tip = tr("Not connected");
        setLoading(false);
        break;
    default:;
    }

    m_tipLabel->setText(tip);
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

void DeviceSettingsItem::onStopLoadingAnimation()
{
    setLoading(false);
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}

} // namespace bluetooth
} // namespace dcc
