/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#include "networkpanel.h"
#include "constants.h"
#include "widgets/tipswidget.h"
#include "utils.h"
#include "item/devicestatushandler.h"
#include "networkdialog.h"

#include <DHiDPIHelper>
#include <DApplicationHelper>
#include <DDBusSender>
#include <DMenu>

#include <QTimer>
#include <QVBoxLayout>
#include <QAction>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ConnectionSettings>

enum MenuItemKey : int {
    MenuSettings = 1,
    MenuEnable,
    MenuDisable,
    MenuWiredEnable,
    MenuWiredDisable,
    MenuWirelessEnable,
    MenuWirelessDisable,
};

NetworkPanel::NetworkPanel(QObject *parent)
    : QObject(parent)
    , m_wirelessScanTimer(new QTimer(this))
    , m_tipsWidget(new Dock::TipsWidget())
    , m_switchWire(true)
    , m_mainWidget(nullptr)
{
    initUi();
    initConnection();
}

NetworkPanel::~NetworkPanel()
{
}

void NetworkPanel::setMainWidget(QWidget *mainWidget)
{
    m_mainWidget = mainWidget;
}

void NetworkPanel::initUi()
{
    m_tipsWidget->setVisible(false);
}

void NetworkPanel::initConnection()
{
    // 主题发生变化触发的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NetworkPanel::onUpdatePlugView);

    // 连接信号
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceAdded, this, &NetworkPanel::onDeviceAdded);
    connect(networkController, &NetworkController::deviceRemoved, this, &NetworkPanel::onUpdatePlugView);
    connect(networkController, &NetworkController::connectivityChanged, this, &NetworkPanel::onUpdatePlugView);

    int wirelessScanInterval = Utils::SettingValue("com.deepin.dde.dock", QByteArray(), "wireless-scan-interval", 10).toInt() * 1000;
    m_wirelessScanTimer->setInterval(wirelessScanInterval);
    const QGSettings *gsetting = Utils::SettingsPtr("com.deepin.dde.dock", QByteArray(), this);
    if (gsetting)
        connect(gsetting, &QGSettings::changed, [ = ](const QString &key) {
            if (key == "wireless-scan-interval") {
                int wirelessScanInterval = gsetting->get("wireless-scan-interval").toInt() * 1000;
                m_wirelessScanTimer->setInterval(wirelessScanInterval);
            }
        });
    connect(m_wirelessScanTimer, &QTimer::timeout, [ = ] {
        QList<NetworkDeviceBase *> devices = networkController->devices();
        for (NetworkDeviceBase *device : devices) {
            if (device->deviceType() == DeviceType::Wireless) {
                WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
                wirelessDevice->scanNetwork();
            }
        }
    });

    QTimer::singleShot(100, this, [ = ] {
        onDeviceAdded(networkController->devices());
    });
}

void NetworkPanel::updatePluginState()
{
    m_pluginState = DeviceStatusHandler::pluginState();
}

PluginState NetworkPanel::getPluginState()
{
    return m_pluginState;
}

QStringList NetworkPanel::ipTipsMessage(const DeviceType &devType)
{
    DeviceType type = static_cast<DeviceType>(devType);
    QStringList tipMessage;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices) {
        if (device->deviceType() != type)
            continue;

        QString ipv4 = device->ipv4();
        if (ipv4.isEmpty())
            continue;

        tipMessage << QString("%1: %2").arg(device->deviceName()).arg(ipv4);
    }

    return tipMessage;
}

void NetworkPanel::updateTooltips()
{
    switch (m_pluginState) {
    case PluginState::Connected: {
        QStringList textList;
        textList << ipTipsMessage(DeviceType::Wireless) << ipTipsMessage(DeviceType::Wired);
        m_tipsWidget->setTextList(textList);
        break;
    }
    case PluginState::WirelessConnected:
        m_tipsWidget->setTextList(ipTipsMessage(DeviceType::Wireless));
        break;
    case PluginState::WiredConnected:
        m_tipsWidget->setTextList(ipTipsMessage(DeviceType::Wired));
        break;
    case PluginState::Disabled:
    case PluginState::WirelessDisabled:
    case PluginState::WiredDisabled:
        m_tipsWidget->setText(tr("Device disabled"));
        break;
    case PluginState::Unknow:
    case PluginState::Nocable:
        m_tipsWidget->setText(tr("Network cable unplugged"));
        break;
    case PluginState::Disconnected:
    case PluginState::WirelessDisconnected:
    case PluginState::WiredDisconnected:
        m_tipsWidget->setText(tr("Not connected"));
        break;
    case PluginState::Connecting:
    case PluginState::WirelessConnecting:
    case PluginState::WiredConnecting:
        m_tipsWidget->setText(tr("Connecting"));
        break;
    case PluginState::ConnectNoInternet:
    case PluginState::WirelessConnectNoInternet:
    case PluginState::WiredConnectNoInternet:
        m_tipsWidget->setText(tr("Connected but no Internet access"));
        break;
    case PluginState::Failed:
    case PluginState::WirelessFailed:
    case PluginState::WiredFailed:
        m_tipsWidget->setText(tr("Connection failed"));
        break;
    case PluginState::WiredIpConflicted:
    case PluginState::WirelessIpConflicted:
        m_tipsWidget->setText(tr("IP conflict"));
        break;
    }
}

int NetworkPanel::deviceCount(const DeviceType &devType) const
{
    // 获取指定的设备类型的设备数量
    int count = 0;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *dev : devices)
        if (dev->deviceType() == static_cast<DeviceType>(devType))
            count++;

    return count;
}

void NetworkPanel::onDeviceAdded(QList<NetworkDeviceBase *> devices)
{
    // 处理新增设备的信号
    for (NetworkDeviceBase *device : devices) {
        // 当网卡连接状态发生变化的时候重新绘制任务栏的图标
        connect(device, &NetworkDeviceBase::deviceStatusChanged, this, &NetworkPanel::onUpdatePlugView);
        connect(device, &NetworkDeviceBase::activeConnectionChanged, this, &NetworkPanel::onUpdatePlugView);

        emit addDevice(device->path());
        switch (device->deviceType()) {
        case DeviceType::Wired: {
            WiredDevice *wiredDevice = static_cast<WiredDevice *>(device);

            connect(wiredDevice, &WiredDevice::connectionAdded, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &WiredDevice::connectionRemoved, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &WiredDevice::connectionPropertyChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::deviceStatusChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::enableChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wiredDevice, &NetworkDeviceBase::connectionChanged, this, &NetworkPanel::onUpdatePlugView);
        } break;
        case DeviceType::Wireless: {
            WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);

            connect(wirelessDevice, &WirelessDevice::networkAdded, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::networkRemoved, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::enableChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::connectionChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::hotspotEnableChanged, this, &NetworkPanel::onUpdatePlugView);
            connect(wirelessDevice, &WirelessDevice::activeConnectionChanged, this, &NetworkPanel::onUpdatePlugView);

            wirelessDevice->scanNetwork();
        } break;
        default:
            break;
        }
    }

    onUpdatePlugView();
}

void NetworkPanel::invokeMenuItem(const QString &menuId)
{
    switch (menuId.toInt()) {
    case MenuItemKey::MenuEnable:
        setDeviceEnabled(DeviceType::Wired, true);
        setDeviceEnabled(DeviceType::Wireless, true);
        break;
    case MenuItemKey::MenuDisable:
        setDeviceEnabled(DeviceType::Wired, false);
        setDeviceEnabled(DeviceType::Wireless, false);
        break;
    case MenuItemKey::MenuWiredEnable:
        setDeviceEnabled(DeviceType::Wired, true);
        break;
    case MenuItemKey::MenuWiredDisable:
        setDeviceEnabled(DeviceType::Wired, false);
        break;
    case MenuItemKey::MenuWirelessEnable:
        setDeviceEnabled(DeviceType::Wireless, true);
        break;
    case MenuItemKey::MenuWirelessDisable:
        setDeviceEnabled(DeviceType::Wireless, false);
        break;
    case MenuItemKey::MenuSettings:
        DDBusSender()
            .service("com.deepin.dde.ControlCenter")
            .interface("com.deepin.dde.ControlCenter")
            .path("/com/deepin/dde/ControlCenter")
            .method(QString("ShowModule"))
            .arg(QString("network"))
            .call();
        break;
    default:
        break;
    }
}

bool NetworkPanel::needShowControlCenter()
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    // 如果没有网络设备，则直接唤起控制中心
    if (devices.size() == 0)
        return true;

    for (NetworkDeviceBase *device : devices) {
        if (!device->isEnabled())
            continue;

        if (device->deviceType() == DeviceType::Wired) {
            WiredDevice *wiredDevice = static_cast<WiredDevice *>(device);
            // 只要有一个有线网卡存在连接列表，就让其弹出网络列表
            if (!wiredDevice->items().isEmpty())
                return false;
        } else if (device->deviceType() == DeviceType::Wireless) {
            WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
            if (!wirelessDevice->accessPointItems().isEmpty())
                return false;
        }
    }

    return true;
}

bool NetworkPanel::deviceEnabled(const DeviceType &deviceType) const
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices)
        if (device->deviceType() == deviceType && device->isEnabled())
            return true;

    return false;
}

void NetworkPanel::setDeviceEnabled(const DeviceType &deviceType, bool enabeld)
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices)
        if (device->deviceType() == deviceType)
            device->setEnabled(enabeld);
}

const QString NetworkPanel::contextMenu(bool hasSetting) const
{
    int wiredCount = deviceCount(DeviceType::Wired);
    int wirelessCount = deviceCount(DeviceType::Wireless);
    bool wiredEnabled = deviceEnabled(DeviceType::Wired);
    bool wirelessEnabeld = deviceEnabled(DeviceType::Wireless);
    QList<QVariant> items;
    if (wiredCount && wirelessCount) {
        items.reserve(3);
        QMap<QString, QVariant> wireEnable;
        if (wiredEnabled) {
            wireEnable["itemId"] = QString::number(MenuWiredDisable);
            wireEnable["itemText"] = tr("Disable wired connection");
        } else {
            wireEnable["itemId"] = QString::number(MenuWiredEnable);
            wireEnable["itemText"] = tr("Enable wired connection");
        }

        wireEnable["isActive"] = true;
        items.push_back(wireEnable);

        QMap<QString, QVariant> wirelessEnable;
        if (wirelessEnabeld) {
            wirelessEnable["itemText"] = tr("Disable wireless connection");
            wirelessEnable["itemId"] = QString::number(MenuWirelessDisable);
        } else {
            wirelessEnable["itemText"] = tr("Enable wireless connection");
            wirelessEnable["itemId"] = QString::number(MenuWirelessEnable);
        }

        wirelessEnable["isActive"] = true;
        items.push_back(wirelessEnable);
    } else {
        items.reserve(2);
        QMap<QString, QVariant> enable;
        if (wiredEnabled || wirelessEnabeld) {
            enable["itemId"] = QString::number(MenuDisable);
            enable["itemText"] = tr("Disable network");
        } else {
            enable["itemId"] = QString::number(MenuEnable);
            enable["itemText"] = tr("Enable network");
        }

        enable["isActive"] = true;
        items.push_back(enable);
    }
    if (hasSetting) {
        QMap<QString, QVariant> settings;
        settings["itemId"] = QString::number(MenuSettings);
        settings["itemText"] = tr("Network settings");
        settings["isActive"] = true;
        items.push_back(settings);
    }
    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;

    return QJsonDocument::fromVariant(menu).toJson();
}

QWidget *NetworkPanel::itemTips()
{
    return m_tipsWidget;
}

void NetworkPanel::onUpdatePlugView()
{
    updatePluginState();
    updateTooltips();
    emit viewUpdate();
}
