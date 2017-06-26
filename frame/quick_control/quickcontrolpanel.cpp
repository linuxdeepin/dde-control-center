#include "quickcontrolpanel.h"
#include "basicsettingspage.h"
#include "quickswitchbutton.h"
#include "vpn/vpncontrolpage.h"
#include "display/displaycontrolpage.h"
#include "wifi/wifipage.h"
#include "bluetooth/bluetoothlist.h"

#include "network/networkmodel.h"
#include "network/networkworker.h"

#include "display/displaymodel.h"
#include "display/displayworker.h"

#include "bluetooth/bluetoothmodel.h"
#include "bluetooth/bluetoothworker.h"
#include "bluetooth/adapter.h"

#include <QVBoxLayout>

using namespace dcc;
using dcc::network::NetworkModel;
using dcc::network::NetworkWorker;
using dcc::network::NetworkDevice;
using dcc::display::DisplayModel;
using dcc::display::DisplayWorker;
using dcc::bluetooth::BluetoothModel;
using dcc::bluetooth::BluetoothWorker;

QuickControlPanel::QuickControlPanel(QWidget *parent)
    : QWidget(parent),

      m_itemStack(new QStackedLayout)
{
    m_networkModel = new NetworkModel(this);
    m_networkWorker = new NetworkWorker(m_networkModel, this);

    m_displayModel = new DisplayModel(this);
    m_displayWorker = new DisplayWorker(m_displayModel, this);

    WifiPage *wifiPage = new WifiPage(m_networkModel);

    m_bluetoothModel = new BluetoothModel(this);
    m_bluetoothWorker = new BluetoothWorker(m_bluetoothModel);
    m_bluetoothWorker->activate();

    BluetoothList *bluetoothList = new BluetoothList(m_bluetoothModel);

    DisplayControlPage *displayPage = new DisplayControlPage(m_displayModel);

    VpnControlPage *vpnPage = new VpnControlPage(m_networkModel);

    m_itemStack->addWidget(new BasicSettingsPage);
    m_itemStack->addWidget(bluetoothList);
    m_itemStack->addWidget(vpnPage);
    m_itemStack->addWidget(wifiPage);
    m_itemStack->addWidget(displayPage);

    m_btSwitch = new QuickSwitchButton(1, "bluetooth");
    m_vpnSwitch = new QuickSwitchButton(2, "VPN");
    m_wifiSwitch = new QuickSwitchButton(3, "wifi");

    m_detailSwitch = new QuickSwitchButton(0, "all_settings");
    QuickSwitchButton *displaySwitch = new QuickSwitchButton(4, "display");

    m_btSwitch->setObjectName("QuickSwitchBluetooth");
    m_btSwitch->setAccessibleName("QuickSwitchBluetooth");
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
    m_switchs.append(m_btSwitch);
    m_switchs.append(m_vpnSwitch);
    m_switchs.append(m_wifiSwitch);
    m_switchs.append(displaySwitch);

    QHBoxLayout *btnsLayout = new QHBoxLayout;
    btnsLayout->addWidget(m_btSwitch);
    btnsLayout->addWidget(m_vpnSwitch);
    btnsLayout->addWidget(m_wifiSwitch);
    btnsLayout->addWidget(displaySwitch);

    btnsLayout->addWidget(m_detailSwitch);
    btnsLayout->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(m_itemStack);
    mainLayout->addLayout(btnsLayout);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(mainLayout);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(m_btSwitch, &QuickSwitchButton::hovered, m_itemStack, &QStackedLayout::setCurrentIndex);
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

    connect(m_btSwitch, &QuickSwitchButton::checkedChanged, this, &QuickControlPanel::onBluetoothButtonClicked);
    connect(m_bluetoothWorker, &BluetoothWorker::deviceEnableChanged, this, &QuickControlPanel::onBluetoothDeviceEnableChanged);
    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this, &QuickControlPanel::onBluetoothDeviceListChanged);
    connect(m_networkModel, &NetworkModel::deviceEnableChanged, this, &QuickControlPanel::onNetworkDeviceEnableChanged);
    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &QuickControlPanel::onNetworkDeviceListChanged);
    connect(m_wifiSwitch, &QuickSwitchButton::checkedChanged, this, &QuickControlPanel::onWirelessButtonClicked);
    connect(wifiPage, &WifiPage::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(wifiPage, &WifiPage::requestDeviceApList, m_networkWorker, &NetworkWorker::queryAccessPoints);
    connect(wifiPage, &WifiPage::requestActivateAccessPoint, m_networkWorker, &NetworkWorker::activateAccessPoint);
    connect(wifiPage, &WifiPage::requestDeactivateConnection, m_networkWorker, &NetworkWorker::deactiveConnection);
    connect(wifiPage, &WifiPage::requestConnectHidden, this, &QuickControlPanel::requestPage);

    connect(m_displayModel, &DisplayModel::monitorListChanged, [=] { displaySwitch->setVisible(m_displayModel->monitorList().size() > 1); });
    connect(displayPage, &DisplayControlPage::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(displayPage, &DisplayControlPage::requestOnlyMonitor, [=](const QString &name) { m_displayWorker->switchMode(SINGLE_MODE, name); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestDuplicateMode, [=] { m_displayWorker->switchMode(MERGE_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestExtendMode, [=] { m_displayWorker->switchMode(EXTEND_MODE); m_displayWorker->saveChanges(); });
    connect(displayPage, &DisplayControlPage::requestConfig, m_displayWorker, &DisplayWorker::switchConfig);
    connect(displayPage, &DisplayControlPage::requestCustom, [=] { emit requestPage("display", QString()); });

    connect(bluetoothList, &BluetoothList::mouseLeaveView, this, [=] { m_itemStack->setCurrentIndex(0); });
    connect(bluetoothList, &BluetoothList::requestConnect, m_bluetoothWorker, &bluetooth::BluetoothWorker::connectDevice);
    connect(bluetoothList, &BluetoothList::requestDisConnect, m_bluetoothWorker, &bluetooth::BluetoothWorker::disconnectDevice);
    connect(bluetoothList, &BluetoothList::requestConnectOther, this, &QuickControlPanel::requestPage);
    connect(bluetoothList, &BluetoothList::requestAdapterDiscoverable, m_bluetoothWorker, &bluetooth::BluetoothWorker::setAdapterDiscoverable);

    connect(m_itemStack, &QStackedLayout::currentChanged, this, &QuickControlPanel::onIndexChanged);

    displaySwitch->setVisible(m_displayModel->monitorList().size() > 1);
    m_vpnSwitch->setChecked(m_networkModel->vpnEnabled());
    onNetworkDeviceEnableChanged();
    onNetworkDeviceListChanged();
    onNetworkConnectionListChanged();
    onBluetoothDeviceEnableChanged();
    onBluetoothDeviceListChanged();

}

void QuickControlPanel::setAllSettingsVisible(const bool visible)
{
    m_detailSwitch->setVisible(visible);
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

void QuickControlPanel::onIndexChanged(const int index)
{
    for (int i(0); i != m_switchs.size(); ++i)
        m_switchs[i]->setSelected(i == index);
}
