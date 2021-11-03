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
#include "../common-plugin/utils.h"
#include "item/devicestatushandler.h"
#include "imageutil.h"
#include "networkdialog.h"

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/ConnectionSettings>

#include <DHiDPIHelper>
#include <DApplicationHelper>
#include <DDBusSender>
#include <DMenu>

#include <QTimer>
#include <QVBoxLayout>
#include <QAction>
#include "notificationmanager.h"

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wireddevice.h>
#include <wirelessdevice.h>

const QString MenueEnable = "enable";
const QString MenueWiredEnable = "wireEnable";
const QString MenueWirelessEnable = "wirelessEnable";
const QString MenueSettings = "settings";

const QString notifyIconNetworkOffline = "notification-network-offline";
const QString notifyIconWiredConnected = "notification-network-wired-connected";
const QString notifyIconWiredDisconnected = "notification-network-wired-disconnected";
const QString notifyIconWiredError = "notification-network-wired-disconnected";
const QString notifyIconWirelessConnected = "notification-network-wireless-full";
const QString notifyIconWirelessDisconnected = "notification-network-wireless-disconnected";
const QString notifyIconWirelessDisabled = "notification-network-wireless-disabled";
const QString notifyIconWirelessError = "notification-network-wireless-disconnected";
const QString notifyIconVpnConnected = "notification-network-vpn-connected";
const QString notifyIconVpnDisconnected = "notification-network-vpn-disconnected";
const QString notifyIconProxyEnabled = "notification-network-proxy-enabled";
const QString notifyIconProxyDisabled = "notification-network-proxy-disabled";
const QString notifyIconNetworkConnected = "notification-network-wired-connected";
const QString notifyIconNetworkDisconnected = "notification-network-wired-disconnected";
const QString notifyIconMobile2gConnected = "notification-network-mobile-2g-connected";
const QString notifyIconMobile2gDisconnected = "notification-network-mobile-2g-disconnected";
const QString notifyIconMobile3gConnected = "notification-network-mobile-3g-connected";
const QString notifyIconMobile3gDisconnected = "notification-network-mobile-3g-disconnected";
const QString notifyIconMobile4gConnected = "notification-network-mobile-4g-connected";
const QString notifyIconMobile4gDisconnected = "notification-network-mobile-4g-disconnected";
const QString notifyIconMobileUnknownConnected = "notification-network-mobile-unknown-connected";
const QString notifyIconMobileUnknownDisconnected = "notification-network-mobile-unknown-disconnected";

NetworkPanel::NetworkPanel(QWidget *parent)
    : QWidget(parent)
    , m_refreshIconTimer(new QTimer(this))
    , m_switchWireTimer(new QTimer(this))
    , m_wirelessScanTimer(new QTimer(this))
    , m_switchWire(true)
    , m_timeOut(true)
    , m_networkDialog(new NetworkDialog(this))
{
    QDBusConnection::sessionBus().connect("com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront", "Visible", this, SLOT(lockScreen(bool)));
    m_isLockModel = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.dde.lockFront");
    if (!m_isLockModel) {
        dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
    }
    initUi();
    initConnection();
}

NetworkPanel::~NetworkPanel()
{
}

void NetworkPanel::initUi()
{
    setAccessibleName(QStringLiteral("SwitchAuthBtn"));
    setFixedSize(QSize(52, 52));
    setBackgroundRole(DPalette::Button);
    m_refreshIconTimer->setInterval(100);

    setControlBackground();
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

    // 连接超时的信号
    connect(m_switchWireTimer, &QTimer::timeout, [ = ]() {
        m_switchWire = !m_switchWire;
        m_timeOut = true;
    });
    int wirelessScanInterval = Utils::SettingValue("com.deepin.dde.dock", QByteArray(), "wireless-scan-interval", 10).toInt() * 1000;
    m_wirelessScanTimer->setInterval(wirelessScanInterval);
    const QGSettings *gsetting = Utils::SettingsPtr("com.deepin.dde.dock", QByteArray(), this);
    if (gsetting)
        connect(gsetting, &QGSettings::changed, [ = ](const QString & key) {
        if (key == "wireless-scan-interval") {
            int wirelessScanInterval = gsetting->get("wireless-scan-interval").toInt() * 1000;
            m_wirelessScanTimer->setInterval(wirelessScanInterval);
        }
    });
    connect(m_wirelessScanTimer, &QTimer::timeout, [ = ] {
        QList<NetworkDeviceBase *> devices = networkController->devices();
        for (NetworkDeviceBase *device : devices)
        {
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
    switch (m_pluginState) {
    case PluginState::Connected: {
        QStringList textList;
        textList << ipTipsMessage(DeviceType::Wireless) << ipTipsMessage(DeviceType::Wired);
        m_tips = textList.join("\n");
        break;
    }
    case PluginState::WirelessConnected:
        m_tips = ipTipsMessage(DeviceType::Wireless).join("\n");
        break;
    case PluginState::WiredConnected:
        m_tips = ipTipsMessage(DeviceType::Wired).join("\n");
        break;
    case PluginState::Disabled:
    case PluginState::WirelessDisabled:
    case PluginState::WiredDisabled:
        m_tips = tr("Device disabled");
        break;
    case PluginState::Unknow:
    case PluginState::Nocable:
        m_tips = tr("Network cable unplugged");
        break;
    case PluginState::Disconnected:
    case PluginState::WirelessDisconnected:
    case PluginState::WiredDisconnected:
        m_tips = tr("Not connected");
        break;
    case PluginState::Connecting:
    case PluginState::WirelessConnecting:
    case PluginState::WiredConnecting:
        m_tips = tr("Connecting");
        break;
    case PluginState::ConnectNoInternet:
    case PluginState::WirelessConnectNoInternet:
    case PluginState::WiredConnectNoInternet:
        m_tips = tr("Connected but no Internet access");
        break;
    case PluginState::Failed:
    case PluginState::WirelessFailed:
    case PluginState::WiredFailed:
        m_tips = tr("Connection failed");
        break;
    }
}

void NetworkPanel::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    const QRectF &rf = rect();
    const QRectF &rfp = QRectF(m_iconPixmap.rect());
    painter.drawPixmap(rf.center() - rfp.center() / m_iconPixmap.devicePixelRatioF(),
                       m_iconPixmap);
}

void NetworkPanel::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    // 保持横纵比
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }

    refreshIcon();
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

        if (!m_isLockModel) {
            // 登录界面才监听该信号，用于横幅、密码错误处理
            QString devicePath = device->path();
            if (!m_devicePaths.contains(devicePath)) {
                NetworkManager::Device *nmDevice = nullptr;
                if (device->deviceType() == DeviceType::Wireless) {
                    NetworkManager::WirelessDevice *wDevice = new NetworkManager::WirelessDevice(devicePath, device);
                    nmDevice = wDevice;
                    connect(wDevice, &NetworkManager::WirelessDevice::activeAccessPointChanged, this, [this](const QString & ap) {
                        m_lastActiveWirelessDevicePath = static_cast<NetworkManager::WirelessDevice *>(sender())->uni() + ap;
                    });
                } else {
                    nmDevice = new NetworkManager::Device(devicePath, device);
                }
                connect(nmDevice, &NetworkManager::Device::stateChanged, this, &NetworkPanel::onDeviceStatusChanged);
                m_devicePaths.insert(devicePath);
            }
        }

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
    // 有线设备是否可用
    bool wiredEnabled = deviceEnabled(DeviceType::Wired);
    // 无线设备是否可用
    bool wirelessEnabeld = deviceEnabled(DeviceType::Wireless);
    if (menuId == MenueEnable) {
        setDeviceEnabled(DeviceType::Wired, !wiredEnabled);
        setDeviceEnabled(DeviceType::Wireless, !wirelessEnabeld);
    } else if (menuId == MenueWiredEnable) {
        setDeviceEnabled(DeviceType::Wired, !wiredEnabled);
    } else if (menuId == MenueWirelessEnable) {
        setDeviceEnabled(DeviceType::Wireless, !wirelessEnabeld);
    } else if (menuId == MenueSettings) {
        DDBusSender()
        .service("com.deepin.dde.ControlCenter")
        .interface("com.deepin.dde.ControlCenter")
        .path("/com/deepin/dde/ControlCenter")
        .method(QString("ShowModule"))
        .arg(QString("network"))
        .call();
    }
}

bool NetworkPanel::needShowControlCenter()
{
    // 得到有线设备和无线设备的数量
    int wiredCount = deviceCount(DeviceType::Wired);
    int wirelessCount = deviceCount(DeviceType::Wireless);
    bool onlyOneTypeDevice = false;
    if ((wiredCount == 0 && wirelessCount > 0)
            || (wiredCount > 0 && wirelessCount == 0))
        onlyOneTypeDevice = true;

    if (onlyOneTypeDevice) {
        switch (m_pluginState) {
        case PluginState::Unknow:
        case PluginState::Nocable:
        case PluginState::WiredFailed:
        case PluginState::WirelessConnectNoInternet:
        case PluginState::WiredConnectNoInternet:
        case PluginState::WirelessDisconnected:
        case PluginState::WiredDisconnected:
        case PluginState::Disabled:
        case PluginState::WiredDisabled:
            return true;
        default:
            return false;
        }
    } else {
        switch (m_pluginState) {
        case PluginState::Unknow:
        case PluginState::Nocable:
        case PluginState::WiredFailed:
        case PluginState::ConnectNoInternet:
        case PluginState::Disconnected:
        case PluginState::Disabled:
            return true;
        default:
            return false;
        }
    }

    Q_UNREACHABLE();
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

const QString NetworkPanel::contextMenu() const
{
    int wiredCount = deviceCount(DeviceType::Wired);
    int wirelessCount = deviceCount(DeviceType::Wireless);
    bool wiredEnabled = deviceEnabled(DeviceType::Wired);
    bool wirelessEnabeld = deviceEnabled(DeviceType::Wireless);
    QList<QVariant> items;
    if (wiredCount && wirelessCount) {
        items.reserve(3);
        QMap<QString, QVariant> wireEnable;
        wireEnable["itemId"] = MenueWiredEnable;
        if (wiredEnabled)
            wireEnable["itemText"] = tr("Disable wired connection");
        else
            wireEnable["itemText"] = tr("Enable wired connection");

        wireEnable["isActive"] = true;
        items.push_back(wireEnable);

        QMap<QString, QVariant> wirelessEnable;
        wirelessEnable["itemId"] = MenueWirelessEnable;
        if (wirelessEnabeld)
            wirelessEnable["itemText"] = tr("Disable wireless connection");
        else
            wirelessEnable["itemText"] = tr("Enable wireless connection");

        wirelessEnable["isActive"] = true;
        items.push_back(wirelessEnable);
    } else {
        items.reserve(2);
        QMap<QString, QVariant> enable;
        enable["itemId"] = MenueEnable;
        if (wiredEnabled || wirelessEnabeld)
            enable["itemText"] = tr("Disable network");
        else
            enable["itemText"] = tr("Enable network");

        enable["isActive"] = true;
        items.push_back(enable);
    }

    QMap<QString, QVariant> menu;
    menu["items"] = items;
    menu["checkableMenu"] = false;
    menu["singleCheck"] = false;

    return QJsonDocument::fromVariant(menu).toJson();
}

void NetworkPanel::contextMenuEvent(QContextMenuEvent *event)
{
    DMenu menu(this);
    bool wiredEnabled = deviceEnabled(DeviceType::Wired);
    bool wirelessEnabeld = deviceEnabled(DeviceType::Wireless);
    if (wiredEnabled && wirelessEnabeld) {
        QAction *wiredCtrl = menu.addAction(QString());
        wiredCtrl->setData(MenueWiredEnable);
        if (wiredEnabled) {
            wiredCtrl->setText(tr("Disable wired connection"));
        } else {
            wiredCtrl->setText(tr("Enable wired connection"));
        }

        QAction *wirelessCtrl = menu.addAction(QString());
        wirelessCtrl->setData(MenueWirelessEnable);
        if (wirelessEnabeld)
            wirelessCtrl->setText(tr("Disable wireless connection"));
        else
            wirelessCtrl->setText(tr("Enable wireless connection"));

    } else {
        QAction *networkCtrl = menu.addAction(QString());
        networkCtrl->setData(MenueEnable);
        if (wiredEnabled || wirelessEnabeld)
            networkCtrl->setText(tr("Disable network"));
        else
            networkCtrl->setText(tr("Enable network"));
    }
    QAction *action = menu.exec(cursor().pos());
    if (action) {
        invokeMenuItem(action->data().toString());
    }
}

QString NetworkPanel::itemTips()
{
    return m_tips;
}

bool NetworkPanel::hasDevice()
{
    return NetworkController::instance()->devices().size() > 0;
}

void NetworkPanel::refreshIcon()
{
    setControlBackground();

    QString stateString;
    QString iconString;
    const auto ratio = devicePixelRatioF();
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
            if (height() <= PLUGIN_BACKGROUND_MIN_SIZE
                    && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
                iconString.append(PLUGIN_MIN_ICON_NAME);
            m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
            update();
            return;
        } else {
            m_refreshIconTimer->start(200);
            const int index = QTime::currentTime().msec() / 200 % 10;
            const int num = index + 1;
            iconString = QString("network-wired-symbolic-connecting%1").arg(num);
            if (height() <= PLUGIN_BACKGROUND_MIN_SIZE
                    && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
                iconString.append(PLUGIN_MIN_ICON_NAME);
            m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
            update();
            return;
        }
    }
    case PluginState::WirelessConnecting: {
        m_refreshIconTimer->start();
        strength = QTime::currentTime().msec() / 10 % 100;
        stateString = getStrengthStateString(strength);
        iconString = QString("wireless-%1-symbolic").arg(stateString);
        if (height() <= PLUGIN_BACKGROUND_MIN_SIZE
                && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
            iconString.append(PLUGIN_MIN_ICON_NAME);
        m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
        update();
        return;
    }
    case PluginState::WiredConnecting: {
        m_refreshIconTimer->start(200);
        const int index = QTime::currentTime().msec() / 200 % 10;
        const int num = index + 1;
        iconString = QString("network-wired-symbolic-connecting%1").arg(num);
        if (height() <= PLUGIN_BACKGROUND_MIN_SIZE
                && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
            iconString.append(PLUGIN_MIN_ICON_NAME);
        m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
        update();
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

    if (height() <= PLUGIN_BACKGROUND_MIN_SIZE && DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        iconString.append(PLUGIN_MIN_ICON_NAME);

    m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);

    update();
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

void NetworkPanel::setControlBackground()
{
    QPalette backgroud;
    QColor separatorColor;
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType)
        backgroud.setColor(QPalette::Background, QColor(255, 255, 255, 0.03 * 255));
    else
        backgroud.setColor(QPalette::Background, QColor(0, 0, 0, 0.03 * 255));
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
    QTimer::singleShot(500, this, [ = ] () mutable {
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
// 连接中
static bool isDeviceStateInActivating(NetworkManager::Device::State state)
{
    return state >= NetworkManager::Device::Preparing && state <= NetworkManager::Device::Activated;
}

void NetworkPanel::onDeviceStatusChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    if (m_isLockModel) {
        return;
    }
    NetworkManager::Device *d = qobject_cast<NetworkManager::Device *>(sender());
    NetworkManager::Device::Ptr device(new NetworkManager::Device(d->uni()));
    QString path = device->uni();
    switch (newstate) {
    case NetworkManager::Device::State::Preparing: { // 正在连接
        NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
        QString icon;
        switch (device->type()) {
        case NetworkManager::Device::Type::Ethernet:
            icon = notifyIconWiredConnected;
            break;
        case NetworkManager::Device::Type::Wifi:
            icon = notifyIconWirelessConnected;
            break;
        }
        if (oldstate == NetworkManager::Device::State::Disconnected && !conn.isNull()) {
            NotificationManager::Notify(icon, "", tr("Connecting %1").arg(conn->id()));
        }
    } break;
    case NetworkManager::Device::State::Activated: { // 连接成功
        NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
        if (conn.isNull()) {
            return;
        }
        QString icon;
        switch (device->type()) {
        case NetworkManager::Device::Type::Ethernet:
            icon = notifyIconWiredConnected;
            break;
        case NetworkManager::Device::Type::Wifi:
            icon = notifyIconWirelessConnected;
            break;
        }
        NotificationManager::Notify(icon, "", tr("%1 connected").arg(conn->id()));
    } break;
    case NetworkManager::Device::State::Failed: {
        if (device->type() == NetworkManager::Device::Wifi) {
            switch (reason) {
            case NetworkManager::Device::NoSecretsReason:
            case NetworkManager::Device::ConfigUnavailableReason:
                m_networkDialog->setConnectWireless(m_lastActiveWirelessDevicePath);
                onClick();
                break;
            default:
                break;
            }
        }
    } // 没有break，还需要继续处理横幅
    case NetworkManager::Device::State::Disconnected:
    case NetworkManager::Device::State::NeedAuth:
    case NetworkManager::Device::State::Unmanaged:
    case NetworkManager::Device::State::Unavailable: {
        NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
        if (conn.isNull()) {
            return;
        }
        if (reason == NetworkManager::Device::StateChangeReason::DeviceRemovedReason) {
            return;
        }

        // ignore if device's old state is not available
        if (!(oldstate > NetworkManager::Device::State::Unavailable)) {
            qDebug("no notify, old state is not available");
            return;
        }

        if (!NetworkManager::isNetworkingEnabled()) {
            qDebug("no notify, network disabled");
            return;
        }

        // ignore invalid reasons
        if (reason == NetworkManager::Device::StateChangeReason::UnknownReason) {
            qDebug("no notify, device state reason invalid");
            return;
        }

        QString icon;
        QString msg;
        switch (device->type()) {
        case NetworkManager::Device::Type::Ethernet:
            icon = notifyIconWiredDisconnected;
            break;
        case NetworkManager::Device::Type::Wifi:
            icon = notifyIconWirelessDisconnected;
            break;
        }

        switch (reason) {
        case NetworkManager::Device::StateChangeReason::UserRequestedReason:
            if (newstate == NetworkManager::Device::State::Disconnected) {
                msg = tr("%1 disconnected").arg(conn->id());
            }
            break;
        case NetworkManager::Device::StateChangeReason::NewActivation:
        case NetworkManager::Device::StateChangeReason::ConfigUnavailableReason:
            switch (conn->type()) {
            case NetworkManager::ConnectionSettings::ConnectionType::Wired:
                msg = tr("Unable to connect %1, please check your router or net cable.").arg(conn->id());
                break;
            case NetworkManager::ConnectionSettings::ConnectionType::Wireless:
                msg = tr("Unable to connect %1, please keep closer to the wireless router").arg(conn->id());
                break;
            }
            break;
        case NetworkManager::Device::StateChangeReason::AuthSupplicantDisconnectReason:
            if (oldstate == NetworkManager::Device::State::ConfiguringHardware && newstate == NetworkManager::Device::State::NeedAuth) {
                msg = tr("Connection failed, unable to connect %1, wrong password").arg(conn->id());
            }
            break;
        case NetworkManager::Device::StateChangeReason::CarrierReason:
            if (device->type() == NetworkManager::Device::Ethernet) {
                qDebug("unplugged device is ethernet");
                msg = tr("%1 disconnected").arg(conn->id());
            }
            break;
        case NetworkManager::Device::StateChangeReason::NoSecretsReason:
            msg = tr("Password is required to connect %1").arg(conn->id());
            break;
        case NetworkManager::Device::StateChangeReason::SsidNotFound:
            msg = tr("The %1 802.11 WLAN network could not be found").arg(conn->id());
            break;
        }
        if (!msg.isEmpty()) {
            NotificationManager::Notify(icon, "", msg);
        }
    } break;
    }
}

void NetworkPanel::onClick()
{
    QPoint point = this->mapToGlobal(QPoint(width() / 2, 0));
    m_networkDialog->show(point.x(), point.y(), Dock::Position::Bottom, !m_isLockModel);
}

void NetworkPanel::lockScreen(bool lock)
{
    m_isLockModel = true;
    m_isLockScreen = lock;
}
