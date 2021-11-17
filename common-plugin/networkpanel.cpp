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
#include "imageutil.h"
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
    , m_refreshIconTimer(new QTimer(this))
    , m_switchWireTimer(new QTimer(this))
    , m_wirelessScanTimer(new QTimer(this))
    , m_tipsWidget(new Dock::TipsWidget())
    , m_switchWire(true)
    , m_timeOut(true)
    , m_networkInter(new DbusNetwork("com.deepin.daemon.Network", "/com/deepin/daemon/Network", QDBusConnection::sessionBus(), this))
    , m_detectConflictTimer(new QTimer(this))
    , m_ipConflict(false)
    , m_ipConflictChecking(false)
    , m_greeterStyle(false)
{
    initUi();
    initConnection();
}

NetworkPanel::~NetworkPanel()
{
}

void NetworkPanel::initUi()
{
    m_refreshIconTimer->setInterval(100);
    m_tipsWidget->setVisible(false);
}

void NetworkPanel::initConnection()
{
    // 定期更新网络状态图标
    connect(m_refreshIconTimer, &QTimer::timeout, this, &NetworkPanel::refreshIcon);

    // 主题发生变化触发的信号
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::themeTypeChanged, this, &NetworkPanel::onUpdatePlugView);

    // 连接信号
    NetworkController *networkController = NetworkController::instance();
    connect(networkController, &NetworkController::deviceAdded, this, &NetworkPanel::onDeviceAdded);
    connect(networkController, &NetworkController::deviceRemoved, this, &NetworkPanel::onUpdatePlugView);
    connect(networkController, &NetworkController::connectivityChanged, this, &NetworkPanel::onUpdatePlugView);
    connect(m_networkInter, &DbusNetwork::IPConflict, this, &NetworkPanel::onIPConfllict);
    connect(this, &NetworkPanel::sendIpConflictDect, this, &NetworkPanel::onSendIpConflictDect);
    connect(m_detectConflictTimer, &QTimer::timeout, this, &NetworkPanel::onDetectConflict);

    // 连接超时的信号
    connect(m_switchWireTimer, &QTimer::timeout, [ = ]() {
        m_switchWire = !m_switchWire;
        m_timeOut = true;
    });

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

void NetworkPanel::getPluginState()
{
    // 所有设备状态叠加
    QList<int> status;
    m_pluginState = DeviceStatusHandler::pluginState();
    switch (m_pluginState) {
    case PluginState::Unknow:
    case PluginState::Disabled:
    case PluginState::Connected:
    case PluginState::Disconnected:
    case PluginState::ConnectNoInternet:
    case PluginState::WirelessDisabled:
    case PluginState::WiredDisabled:
    case PluginState::WirelessConnected:
    case PluginState::WiredConnected:
    case PluginState::WirelessDisconnected:
    case PluginState::WiredDisconnected:
    case PluginState::WirelessConnecting:
    case PluginState::WiredConnecting:
    case PluginState::WirelessConnectNoInternet:
    case PluginState::WiredConnectNoInternet:
    case PluginState::WiredFailed:
    case PluginState::Nocable:
        m_switchWireTimer->stop();
        m_timeOut = true;
        break;
    case PluginState::Connecting:
        // 启动2s切换计时,只有当计时器记满则重新计数
        if (m_timeOut) {
            m_switchWireTimer->start(2000);
            m_timeOut = false;
        }
        break;
    default:
        break;
    }
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
    if (m_ipConflict) {
        m_tipsWidget->setText(tr("IP conflict"));
        return;
    }

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
    }
}

QString NetworkPanel::getStrengthStateString(int strength)
{
    if (5 >= strength)
        return "0";

    if (30 >= strength)
        return "20";

    if (55 >= strength)
        return "40";

    if (65 >= strength)
        return "60";

    return "80";
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

void NetworkPanel::refreshIcon()
{
    QString stateString;
    QString iconString;
    const auto ratio = 1.0;
    int iconSize = PLUGIN_ICON_MAX_SIZE;
    int strength = 0;

    switch (m_pluginState) {
    case PluginState::Disabled:
    case PluginState::WirelessDisabled:
        stateString = "disabled";
        iconString = QString("wireless-%1-symbolic").arg(stateString);
        break;
    case PluginState::WiredDisabled:
        stateString = "disabled";
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    case PluginState::Connected:
    case PluginState::WirelessConnected:
        strength = getStrongestAp();
        stateString = getStrengthStateString(strength);
        iconString = QString("wireless-%1-symbolic").arg(stateString);
        break;
    case PluginState::WiredConnected:
        stateString = "online";
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    case PluginState::Disconnected:
    case PluginState::WirelessDisconnected:
        stateString = "0";
        iconString = QString("wireless-%1-symbolic").arg(stateString);
        break;
    case PluginState::WiredDisconnected:
        stateString = "none";
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    case PluginState::Connecting: {
        m_refreshIconTimer->start();
        if (m_switchWire) {
            strength = QTime::currentTime().msec() / 10 % 100;
            stateString = getStrengthStateString(strength);
            iconString = QString("wireless-%1-symbolic").arg(stateString);
            if (!m_greeterStyle && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
                iconString.append(PLUGIN_MIN_ICON_NAME);
            m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
            emit iconChange();
            return;
        } else {
            m_refreshIconTimer->start(200);
            const int index = QTime::currentTime().msec() / 200 % 10;
            const int num = index + 1;
            iconString = QString("network-wired-symbolic-connecting%1").arg(num);
            if (!m_greeterStyle && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
                iconString.append(PLUGIN_MIN_ICON_NAME);
            m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
            emit iconChange();
            return;
        }
    }
    case PluginState::WirelessConnecting: {
        m_refreshIconTimer->start();
        strength = QTime::currentTime().msec() / 10 % 100;
        stateString = getStrengthStateString(strength);
        iconString = QString("wireless-%1-symbolic").arg(stateString);
        if (!m_greeterStyle && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
            iconString.append(PLUGIN_MIN_ICON_NAME);
        m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
        emit iconChange();
        return;
    }
    case PluginState::WiredConnecting: {
        m_refreshIconTimer->start(200);
        const int index = QTime::currentTime().msec() / 200 % 10;
        const int num = index + 1;
        iconString = QString("network-wired-symbolic-connecting%1").arg(num);
        if (!m_greeterStyle && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
            iconString.append(PLUGIN_MIN_ICON_NAME);
        m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
        emit iconChange();
        return;
    }
    case PluginState::ConnectNoInternet:
    case PluginState::WirelessConnectNoInternet: {
        // 无线已连接但无法访问互联网 offline
        stateString = "offline";
        iconString = QString("network-wireless-%1-symbolic").arg(stateString);
        break;
    }
    case PluginState::WiredConnectNoInternet: {
        stateString = "warning";
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    }
    case PluginState::WiredFailed: {
        // 有线连接失败none变为offline
        stateString = "offline";
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    }
    case PluginState::Unknow:
    case PluginState::Nocable: {
        stateString = "error"; // 待图标 暂用错误图标
        iconString = QString("network-%1-symbolic").arg(stateString);
        break;
    }
    case PluginState::WirelessFailed:
    case PluginState::Failed: {
        // 无线连接失败改为 disconnect
        stateString = "disconnect";
        iconString = QString("wireless-%1").arg(stateString);
        break;
    }
    }

    if (m_ipConflict) {
        //如果无线连接有IP冲突，则显示已连接但是无法访问网络的图标
        QStringList wirelessIpList = getActiveWirelessList();
        bool wirelessConflicted = false;
        if (wirelessIpList.size() > 0) {
            foreach (auto ip, wirelessIpList) {
                if (m_conflictMap.keys().contains(ip)) {
                    stateString = "offline";
                    iconString = QString("network-wireless-%1-symbolic").arg(stateString);
                    wirelessConflicted = true;
                    break;
                }
            }
        }
        // 如果有线连接有IP冲突，则显示有线连接断开的图标
        QStringList wiredIpList = getActiveWiredList();
        if (!wirelessConflicted && wiredIpList.size() > 0) {
            foreach (auto ip, wiredIpList) {
                if (m_conflictMap.keys().contains(ip)) {
                    stateString = "offline";
                    iconString = QString("network-%1-symbolic").arg(stateString);
                    break;
                }
            }
        }
    }
    m_refreshIconTimer->stop();

    if (!m_greeterStyle && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        iconString.append(PLUGIN_MIN_ICON_NAME);

    m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);

    emit iconChange();
}

QPixmap NetworkPanel::icon()
{
    return m_iconPixmap;
}

QStringList NetworkPanel::getIPList(const DeviceType &deviceType) const
{
    QStringList ipList;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices) {
        if (device->deviceType() != deviceType)
            continue;

        if (device->ipv4().isEmpty())
            continue;

        ipList << device->ipv4();
    }
    return ipList;
}

QStringList NetworkPanel::getActiveWiredList() const
{
    return getIPList(DeviceType::Wired);
}

QStringList NetworkPanel::getActiveWirelessList() const
{
    return getIPList(DeviceType::Wireless);
}

QStringList NetworkPanel::currentIpList() const
{
    QStringList nativeIpList;

    nativeIpList.append(getActiveWiredList());
    nativeIpList.append(getActiveWirelessList());

    return nativeIpList;
}

void NetworkPanel::onUpdatePlugView()
{
    getPluginState();
    refreshIcon();
    updateTooltips();
}

/**ip冲突以及冲突解除时，更新网络插件显示状态
 * @brief NetworkItem::ipConfllict
 * @param ip 本机的ip地址
 * @param mac 与本机冲突的mac地址，不为空，则冲突，为空则ip冲突解除
 */
void NetworkPanel::onIPConfllict(const QString &ip, const QString &mac)
{
    static int conflictCount = 0;
    static int removeCount = 0;

    QStringList ipList = currentIpList();
    //判断缓存冲突列表中的IP是否在本机IP列表中
    foreach (auto tmpIP, m_conflictMap.keys()) {
        if (!ipList.contains(tmpIP)) {
            m_conflictMap.remove(tmpIP);
        }
    }
    // 如果不是本机ip
    if (!ipList.contains(ip))
        return;

    // mac为空时冲突解除或IP地址不冲突
    if (mac.isEmpty()) {
        // 自检为空，则解除ip冲突状态或者冲突列表为空时，更新状态
        m_conflictMap.remove(ip);

        if (m_conflictMap.isEmpty()) {
            conflictCount = 0;
        }

        if (m_conflictMap.isEmpty() && m_ipConflict) {
            // 确认1次解除
            if (removeCount++ < 1) {
                onDetectConflict();
                return;
            }

            // 当mac为空且map也为空时，立即更新状态会导致文字显示由'ip地址冲突'变为'已连接网络但无法访问互联网'
            // 因为加了次数判断
            m_ipConflict = false;
            m_ipConflictChecking = false;
            m_detectConflictTimer->stop();
            onUpdatePlugView();
            m_conflictMap.clear();
            removeCount = 0;
        }
        return;
    }

    // 缓存冲突ip和mac地址
    m_conflictMap.insert(ip, mac);
    removeCount = 0;

    if (m_conflictMap.size() && !m_ipConflict) {
        // 确认2次
        if (conflictCount++ < 2) {
            onDetectConflict();
            return;
        }

        conflictCount = 0;
        m_ipConflict = true;
        onUpdatePlugView();

        // 有冲突才开启5秒中ip自检，目的是当其他主机主动解除了冲突，我方不知情
        m_detectConflictTimer->start(5000);
    }
}

/**
 * @brief NetworkItem::onSendIpConflictDect 延时发送ip冲突检测
 * @param index 本地ip地址索引号
 */
void NetworkPanel::onSendIpConflictDect(int index)
{
    QTimer::singleShot(500, this, [ = ]() mutable {
        const QStringList ipList = currentIpList();
        if (index >= ipList.size()) {
            m_ipConflictChecking = false;
            return;
        }

        m_networkInter->RequestIPConflictCheck(ipList.at(index), "");

        ++index;
        if (ipList.size() > index)
            emit sendIpConflictDect(index);
        else
            m_ipConflictChecking = false;
    });
}

void NetworkPanel::onDetectConflict()
{
    // ip冲突时发起主动检测，如果解除则更新状态
    if (currentIpList().size() <= 0 || m_ipConflictChecking)
        return;

    m_ipConflictChecking = true;
}

int NetworkPanel::getStrongestAp()
{
    int retStrength = -1;
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices) {
        if (device->deviceType() != DeviceType::Wireless)
            continue;

        WirelessDevice *dev = static_cast<WirelessDevice *>(device);
        AccessPoints *ap = dev->activeAccessPoints();
        if (ap && retStrength < ap->strength())
            retStrength = ap->strength();
    }

    return retStrength;
}

void NetworkPanel::setGreeterStyle(bool greeterStyle)
{
    m_greeterStyle = greeterStyle;
}
