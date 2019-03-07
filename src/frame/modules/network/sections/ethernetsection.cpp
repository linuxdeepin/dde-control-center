/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "ethernetsection.h"
#include "widgets/contentwidget.h"

#include <dspinbox.h>

#include <networkmanagerqt/manager.h>
#include <networkmanagerqt/wireddevice.h>

#define NotBindValue "NotBind"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

EthernetSection::EthernetSection(NetworkManager::WiredSetting::Ptr wiredSetting, QFrame *parent)
    : AbstractSection(tr("Ethernet"), parent),
      m_deviceMac(new ComboBoxWidget(this)),
      m_clonedMac(new LineEditWidget(this)),
      m_customMtuSwitch(new SwitchWidget(this)),
      m_customMtu(new SpinBoxWidget(this)),
      m_wiredSetting(wiredSetting)
{
    // get the macAddress list from all wired devices
    for (auto device : NetworkManager::networkInterfaces()) {
        if (device->type() != NetworkManager::Device::Ethernet) {
            continue;
        }
        NetworkManager::WiredDevice::Ptr wDevice = device.staticCast<NetworkManager::WiredDevice>();
        /* Alt:  permanentHardwareAddress to get real hardware address which is connot be changed */
        const QString &macStr = wDevice->permanentHardwareAddress() + " (" + wDevice->interfaceName() + ")";
        m_macStrMap.insert(macStr, wDevice->permanentHardwareAddress().remove(":"));
    }
    m_macStrMap.insert(tr("Not Bind"), NotBindValue);

    // "^([0-9A-Fa-f]{2}[:-\\.]){5}([0-9A-Fa-f]{2})$"
    m_macAddrRegExp = QRegExp("^([0-9A-Fa-f]{2}[:]){5}([0-9A-Fa-f]{2})$");

    initUI();
    initConnection();
}

EthernetSection::~EthernetSection()
{
}

bool EthernetSection::allInputValid()
{
    const QString &clonedMacStr = m_clonedMac->text();
    if (clonedMacStr.isEmpty()) {
        return true;
    }
    bool matched = m_macAddrRegExp.exactMatch(clonedMacStr);
    m_clonedMac->setIsErr(!matched);
    return matched;
}

void EthernetSection::saveSettings()
{
    QString hwAddr = m_macStrMap.value(m_deviceMac->value());
    if (hwAddr == NotBindValue) {
        hwAddr.clear();
    }
    m_wiredSetting->setMacAddress(QByteArray::fromHex(hwAddr.toUtf8()));

    QString clonedAddr = m_clonedMac->text().remove(":");
    m_wiredSetting->setClonedMacAddress(QByteArray::fromHex(clonedAddr.toUtf8()));

    m_wiredSetting->setMtu(m_customMtuSwitch->checked() ? m_customMtu->spinBox()->value() : 0);

    m_wiredSetting->setInitialized(true);
}

void EthernetSection::initUI()
{
    m_deviceMac->setTitle(tr("Device MAC Addr"));
    for (const QString &key : m_macStrMap.keys()) {
        m_deviceMac->appendOption(key, m_macStrMap.value(key));
    }

    // get the macAddress from existing Settings
    const QString &macAddr = QString(m_wiredSetting->macAddress().toHex()).toUpper();

    if (m_macStrMap.values().contains(macAddr)) {
        m_deviceMac->setCurrent(macAddr);
    } else {
        // set macAddress of the current device to be default value
        m_deviceMac->setCurrent(NotBindValue);
    }

    m_clonedMac->setTitle(tr("Cloned MAC Addr"));
    QString tmp = QString(m_wiredSetting->clonedMacAddress().toHex()).toUpper();
    QString clonedMacAddr;
    if (!tmp.isEmpty()) {
        for (int i = 0; i < tmp.size(); ++i) {
            if (i != 0 && i % 2 == 0) {
                clonedMacAddr.append(":");
            }
            clonedMacAddr.append(tmp.at(i));
        }
    }
    m_clonedMac->setText(clonedMacAddr);

    m_customMtuSwitch->setTitle(tr("Customize MTU"));
    m_customMtuSwitch->setChecked(!(m_wiredSetting->mtu() == 0));

    m_customMtu->setTitle(tr("MTU"));
    m_customMtu->spinBox()->setMinimum(0);
    m_customMtu->spinBox()->setMaximum(10000);
    m_customMtu->spinBox()->setValue(m_wiredSetting->mtu());
    onCostomMtuChanged(m_customMtuSwitch->checked());

    appendItem(m_deviceMac);
    appendItem(m_clonedMac);
    appendItem(m_customMtuSwitch);
    appendItem(m_customMtu);
}

void EthernetSection::initConnection()
{
    connect(m_deviceMac, &ComboBoxWidget::requestPage, this, &EthernetSection::requestNextPage);
    connect(m_clonedMac->textEdit(), &QLineEdit::editingFinished, this, &EthernetSection::allInputValid);
    connect(m_customMtuSwitch, &SwitchWidget::checkedChanged, this, &EthernetSection::onCostomMtuChanged);
}

void EthernetSection::onCostomMtuChanged(const bool enable)
{
    m_customMtu->setVisible(enable);
}
