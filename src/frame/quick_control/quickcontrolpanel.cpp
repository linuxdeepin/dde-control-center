/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"
#include "vpn/vpncontrolpage.h"
#include "display/displaycontrolpage.h"
#include "wifi/wifipage.h"

#include "display/displaymodel.h"
#include "display/displayworker.h"

#ifndef DISABLE_BLUETOOTH
#include "bluetooth/bluetoothlist.h"
#include "bluetooth/bluetoothmodel.h"
#include "bluetooth/bluetoothworker.h"
#include "bluetooth/adapter.h"
#endif

#include <QVBoxLayout>
#include <networkmodel.h>
#include <networkworker.h>

using namespace dcc;
using dde::network::NetworkModel;
using dde::network::NetworkWorker;
using dde::network::NetworkDevice;
using dcc::display::DisplayModel;
using dcc::display::DisplayWorker;

#ifndef DISABLE_BLUETOOTH
using dcc::bluetooth::BluetoothModel;
using dcc::bluetooth::BluetoothWorker;
#endif

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    QHBoxLayout *btnsLayout = new QHBoxLayout;
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_itemStack);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_networkModel = new NetworkModel(this);
    m_networkWorker = new NetworkWorker(m_networkModel, this);

    m_displayModel = new DisplayModel(this);
    m_displayWorker = new DisplayWorker(m_displayModel, this);

    WifiPage *wifiPage = new WifiPage(m_networkModel);

#ifndef DISABLE_BLUETOOTH
    m_bluetoothWorker = &BluetoothWorker::Instance();
    m_bluetoothModel = m_bluetoothWorker->model();
    m_bluetoothWorker->activate();

    BluetoothList *bluetoothList = new BluetoothList(m_bluetoothModel);
#endif

    DisplayControlPage *displayPage = new DisplayControlPage(m_displayModel);

    VpnControlPage *vpnPage = new VpnControlPage(m_networkModel);

    m_basicSettingsPage = new BasicSettingsPage;

    m_itemStack->addWidget(m_basicSettingsPage);
#ifndef DISABLE_BLUETOOTH
    m_itemStack->addWidget(bluetoothList);
#endif
    m_itemStack->addWidget(vpnPage);
    m_itemStack->addWidget(wifiPage);
    m_itemStack->addWidget(displayPage);

#ifndef DISABLE_BLUETOOTH
    m_btSwitch = new QuickSwitchButton(1, "bluetooth");
#endif
    m_vpnSwitch = new QuickSwitchButton(2, "VPN");
    m_wifiSwitch = new QuickSwitchButton(3, "wifi");

    m_detailSwitch = new QuickSwitchButton(0, "all_settings");
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(4, "display");

#ifndef DISABLE_BLUETOOTH
    m_btSwitch->setObjectName("QuickSwitchBluetooth");
    m_btSwitch->setAccessibleName("QuickSwitchBluetooth");
#endif
    m_vpnSwitch->setObjectName("QuickSwitchVPN");
    m_vpnSwitch->setAccessibleName("QuickSwitchVPN");
    m_wifiSwitch->setObjectName("QuickSwitchWiFi");
    m_wifiSwitch->setAccessibleName("QuickSwitchWiFi");

    displaySwitch->setObjectName("QuickSwitchDisplay");
    displaySwitch->setAccessibleName("QuickSwitchDisplay");
    displaySwitch->setCheckable(false);
    displaySwitch->setChecked(true);

    m_detailSwitch->setObjectName("QuickSwitchAllSettings");
    m_detailSwitch->setAccessibleName("QuickSwitchAllSettings");
    m_detailSwitch->setCheckable(false);
    m_detailSwitch->setBackgroundVisible(false);
    m_detailSwitch->setVisible(false);

    m_switchs.append(m_detailSwitch);
#ifndef DISABLE_BLUETOOTH
    m_switchs.append(m_btSwitch);
#endif
    m_switchs.append(m_vpnSwitch);
    m_switchs.append(m_wifiSwitch);
    m_switchs.append(displaySwitch);

#ifndef DISABLE_BLUETOOTH
    btnsLayout->addWidget(m_btSwitch);
#endif
    btnsLayout->addWidget(m_vpnSwitch);
    btnsLayout->addWidget(m_wifiSwitch);
    btnsLayout->addWidget(displaySwitch);

    btnsLayout->addWidget(m_detailSwitch);
    btnsLayout->setContentsMargins(0, 0, 0, 0);

#ifndef DISABLE_BLUETOOTH
    connect(m_btSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
#endif
    connect(m_vpnSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(m_wifiSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);

    connect(displaySwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(m_detailSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
    connect(m_detailSwitch, &QuickSwitchButton::clicked, this, &QuickControlPanel::requestDetailConfig);

    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, m_vpnSwitch, &QuickSwitchButton::setChecked);
    connect(m_networkModel, &NetworkModel::connectionListChanged, this, &QuickControlPanel::onNetworkConnectionListChanged);
    connect(m_vpnSwitch, &QuickSwitchButton::checkedChanged, m_networkWorker, &NetworkWorker::setVpnEnable);
    connect(vpnPage, &VpnControlPage::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(vpnPage, &VpnControlPage::requestActivateConnection, m_networkWorker, &NetworkWorker::activateConnection);
    connect(vpnPage, &VpnControlPage::requestDisconnect, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(m_wifiSwitch, &QuickSwitchButton::hovered, m_networkWorker, &NetworkWorker::requestWirelessScan);


#ifndef DISABLE_BLUETOOTH
    connect(m_btSwitch, &QuickSwitchButton::checkedChanged, this, &QuickControlPanel::onBluetoothButtonClicked);
    connect(m_bluetoothWorker, &BluetoothWorker::deviceEnableChanged, this, &QuickControlPanel::onBluetoothDeviceEnableChanged);
    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this, &QuickControlPanel::onBluetoothDeviceListChanged);
#endif

    connect(m_networkModel, &NetworkModel::deviceEnableChanged, this, &QuickControlPanel::onNetworkDeviceEnableChanged);
    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &QuickControlPanel::onNetworkDeviceListChanged);
    connect(m_wifiSwitch, &QuickSwitchButton::checkedChanged, this, &QuickControlPanel::onWirelessButtonClicked);
    connect(wifiPage, &WifiPage::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(wifiPage, &WifiPage::requestDeviceApList, m_networkWorker, &NetworkWorker::queryAccessPoints);
    connect(wifiPage, &WifiPage::requestActivateAccessPoint, m_networkWorker, &NetworkWorker::activateAccessPoint);
    connect(wifiPage, &WifiPage::requestDeactivateConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(wifiPage, &WifiPage::requestPage, this, &QuickControlPanel::requestPage);

    connect(m_displayModel, &DisplayModel::monitorListChanged, [=] { displaySwitch->setVisible(m_displayModel->monitorList().size() > 1); });
    connect(displayPage, &DisplayControlPage::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(displayPage, &DisplayControlPage::requestOnlyMonitor, [=](const QString &name) { m_displayWorker->switchMode(SINGLE_MODE, name); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestDuplicateMode, [=] { m_displayWorker->switchMode(MERGE_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestExtendMode, [=] { m_displayWorker->switchMode(EXTEND_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestConfig, m_displayWorker, &DisplayWorker::switchConfig);
    connect(displayPage, &DisplayControlPage::requestCustom, [=] { emit requestPage("display", QString(), false); });


#ifndef DISABLE_BLUETOOTH
    connect(bluetoothList, &BluetoothList::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(bluetoothList, &BluetoothList::requestConnect, m_bluetoothWorker, &bluetooth::BluetoothWorker::connectDevice);
    connect(bluetoothList, &BluetoothList::requestDisConnect, m_bluetoothWorker, &bluetooth::BluetoothWorker::disconnectDevice);
    connect(bluetoothList, &BluetoothList::requestDetailPage, this, &QuickControlPanel::requestPage);
    connect(bluetoothList, &BluetoothList::requestAdapterDiscoverable, m_bluetoothWorker, &bluetooth::BluetoothWorker::setAdapterDiscoverable);
#endif

    connect(m_itemStack, &QStackedLayout::currentChanged, this, &QuickControlPanel::onIndexChanged);

    displaySwitch->setVisible(m_displayModel->monitorList().size() > 1);
    m_vpnSwitch->setChecked(m_networkModel->vpnEnabled());
    onNetworkDeviceEnableChanged();
    onNetworkDeviceListChanged();

#ifndef DISABLE_BLUETOOTH
    onNetworkConnectionListChanged();
    onBluetoothDeviceEnableChanged();
    onBluetoothDeviceListChanged();
#endif
}

void QuickControlPanel::setAllSettingsVisible(const bool visible)
{
    m_detailSwitch->setVisible(visible);
}

void QuickControlPanel::setMPRISEnable(const bool enable)
{
    m_basicSettingsPage->setMPRISEnable(enable);
}

void QuickControlPanel::setMPRISPictureEnable(const bool enable)
{
    m_basicSettingsPage->setMPRISPictureEnable(enable);
}

void QuickControlPanel::appear()
{
#ifndef DISABLE_BLUETOOTH
    m_bluetoothWorker->blockDBusSignals(false);
#endif
    m_networkWorker->active();
}

void QuickControlPanel::disappear()
{
#ifndef DISABLE_BLUETOOTH
    m_bluetoothWorker->blockDBusSignals(true);
#endif
    m_networkWorker->deactive();
}

void QuickControlPanel::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);

    m_itemStack->setCurrentIndex(0);
}

void QuickControlPanel::onNetworkDeviceEnableChanged()
{
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() == NetworkDevice::Wireless && dev->enabled())
        {
            m_wifiSwitch->setChecked(true);
            return;
        }
    }

    m_wifiSwitch->setChecked(false);
}

void QuickControlPanel::onNetworkDeviceListChanged()
{
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() == NetworkDevice::Wireless)
        {
            m_wifiSwitch->setVisible(true);
            return;
        }
    }

    m_wifiSwitch->setVisible(false);
    m_itemStack->setCurrentIndex(0);
}

void QuickControlPanel::onNetworkConnectionListChanged()
{
    const bool state = m_networkModel->vpns().size();
    m_vpnSwitch->setVisible(state);

    if (!state)
        m_itemStack->setCurrentIndex(0);
}

void QuickControlPanel::onWirelessButtonClicked()
{
    const bool enable = m_wifiSwitch->checked();

    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() == NetworkDevice::Wireless)
            m_networkWorker->setDeviceEnable(dev->path(), enable);
    }
}

#ifndef DISABLE_BLUETOOTH
void QuickControlPanel::onBluetoothDeviceEnableChanged()
{
    QTimer::singleShot(100, this,[=]{
        for (const Adapter *adapter : m_bluetoothModel->adapters()) {
            if (adapter->powered()) {
                m_btSwitch->setChecked(true);
                return;
            }
        }

        m_btSwitch->setChecked(false);
    });
}

void QuickControlPanel::onBluetoothButtonClicked(const bool checked)
{
    for (const Adapter *adapter : m_bluetoothModel->adapters()) {
        m_bluetoothWorker->setAdapterPowered(adapter, checked);
    }
}

void QuickControlPanel::onBluetoothDeviceListChanged()
{
    for (const Adapter *adapter : m_bluetoothModel->adapters()) {
        Q_UNUSED(adapter);
        m_btSwitch->setVisible(true);
        return;
    }

    m_btSwitch->setVisible(false);
    m_itemStack->setCurrentIndex(0);
}
#endif

void QuickControlPanel::onIndexChanged(const int index)
{
    for (int i(0); i != m_switchs.size(); ++i)
        m_switchs[i]->setSelected(i == index);
}
