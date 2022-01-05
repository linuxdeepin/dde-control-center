/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     Zhang Qipeng <zhangqipeng@uniontech.com>
*
* Maintainer: Zhang Qipeng <zhangqipeng@uniontech.com>
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

#include "network_module.h"
#include "networkpluginhelper.h"
#include "networkdialog.h"
#include "trayicon.h"
#include "notificationmanager.h"

#include <QWidget>
#include <QTime>

#include <DArrowRectangle>

#include <networkcontroller.h>

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WiredSetting>

#define NETWORK_KEY "network-item-key"

using namespace NetworkManager;
NETWORKPLUGIN_USE_NAMESPACE

namespace dss {
namespace module {

NetworkModule::NetworkModule(QObject *parent)
    : QObject(parent)
    , m_lastState(NetworkManager::Device::State::UnknownState)
{
    QDBusConnection::sessionBus().connect("com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront", "Visible", this, SLOT(updateLockScreenStatus(bool)));
    m_isLockModel = (-1 == qAppName().indexOf("greeter"));
    if (!m_isLockModel) {
        dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
    }

    m_networkDialog = new NetworkDialog(this);
    m_networkDialog->setRunReason(NetworkDialog::Lock);
    m_networkHelper = new NetworkPluginHelper(m_networkDialog, this);
    if (!m_isLockModel) {
        m_networkDialog->setRunReason(NetworkDialog::Greeter);
        connect(m_networkHelper, &NetworkPluginHelper::addDevice, this, &NetworkModule::onAddDevice);
    }
    m_networkDialog->runServer(true);
}

QWidget *NetworkModule::content()
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    if(abs(msec - m_clickTime) > 200) {
        m_clickTime = msec;
        emit signalShowNetworkDialog();
        m_networkDialog->show();
    }
    return nullptr;
}

QWidget *NetworkModule::itemWidget() const
{
    TrayIcon *trayIcon = new TrayIcon(m_networkHelper);
    trayIcon->setGreeterStyle(true);
    connect(this, &NetworkModule::signalShowNetworkDialog, trayIcon, &TrayIcon::showNetworkDialog);
    connect(trayIcon, &TrayIcon::signalShowNetworkDialog, this, &NetworkModule::showNetworkDialog);
    connect(m_networkDialog, &NetworkDialog::requestPosition, trayIcon, &TrayIcon::showNetworkDialog);
    // 处理内存
    connect(m_networkHelper, &NetworkPluginHelper::destroyed, trayIcon, &TrayIcon::deleteLater);
    return trayIcon;
}

QWidget *NetworkModule::itemTipsWidget() const
{
    QWidget *itemTips = m_networkHelper->itemTips();
    if (itemTips) {
        QPalette palette = itemTips->palette();
        palette.setColor(QPalette::BrightText, Qt::black);
        itemTips->setPalette(palette);
    }
    return itemTips;
}

const QString NetworkModule::itemContextMenu() const
{
    return m_networkHelper->contextMenu(false);
}

void NetworkModule::invokedMenuItem(const QString &menuId, const bool checked) const
{
    Q_UNUSED(checked);
    m_networkHelper->invokeMenuItem(menuId);
}

void NetworkModule::showNetworkDialog(QWidget *w) const
{
    QPoint point = w->mapToGlobal(QPoint(w->width() / 2, 0));
    m_networkDialog->setPosition(point.x(), point.y(), Dtk::Widget::DArrowRectangle::ArrowBottom);
}

void NetworkModule::updateLockScreenStatus(bool visible)
{
    m_isLockModel = true;
    m_isLockScreen = visible;
    m_networkDialog->runServer(visible);
}

void NetworkModule::onAddDevice(const QString &devicePath)
{
    if (m_isLockModel) {
        return;
    }
    // 登录界面才监听该信号，用于横幅、密码错误处理
    if (!m_devicePaths.contains(devicePath)) {
        Device::Ptr device(new Device(devicePath));
        Device *nmDevice = nullptr;
        if (device->type() == Device::Wifi) {
            NetworkManager::WirelessDevice *wDevice = new NetworkManager::WirelessDevice(devicePath, this);
            nmDevice = wDevice;
            connect(wDevice, &NetworkManager::WirelessDevice::activeAccessPointChanged, this, [this](const QString &ap) {
                m_lastActiveWirelessDevicePath = static_cast<NetworkManager::WirelessDevice *>(sender())->uni() + NetworkManager::AccessPoint(ap).ssid();
            });
        } else if (device->type() == Device::Ethernet) {
            NetworkManager::WiredDevice *wDevice = new NetworkManager::WiredDevice(devicePath, this);
            nmDevice = wDevice;
            addFirstConnection(wDevice);
        }
        if (nmDevice) {
            connect(nmDevice, &NetworkManager::Device::stateChanged, this, &NetworkModule::onDeviceStatusChanged);
            m_devicePaths.insert(devicePath);
        }
    }
}

const QString NetworkModule::connectionMatchName() const
{
    NetworkManager::Connection::List connList = listConnections();
    QStringList connNameList;
    int connSuffixNum = 1;

    for (NetworkManager::Connection::Ptr conn : connList) {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
            connNameList.append(conn->name());
    }

    QString matchConnName = tr("Wired Connection");
    if (!connNameList.contains(matchConnName))
        return matchConnName;

    matchConnName = QString(tr("Wired Connection")) + QString(" %1");
    for (int i = 1; i <= connNameList.size(); ++i) {
        if (!connNameList.contains(matchConnName.arg(i))) {
            connSuffixNum = i;
            break;
        }
        if (i == connNameList.size())
            connSuffixNum = i + 1;
    }

    return matchConnName.arg(connSuffixNum);
}

bool NetworkModule::hasConnection(NetworkManager::WiredDevice *nmDevice, NetworkManager::Connection::List &unSaveDevices)
{
    bool connIsEmpty = false;
    // 获取所有的连接列表,遍历连接列表,如果当前连接中的MAC地址不为空且不等于当前网卡的MAC地址，则认为它不是当前网卡的连接
    // 在获取所有连接之前，需要手动调用一下当前设备的availableConnections接口，否则获取到的连接列表就不是最新的(具体原因待查)。
    nmDevice->availableConnections();
    NetworkManager::Connection::List connList = listConnections();
    for (NetworkManager::Connection::Ptr conn : connList) {
        WiredSetting::Ptr settings = conn->settings()->setting(Setting::Wired).staticCast<WiredSetting>();
        // 如果当前连接的MAC地址不为空且连接的MAC地址不等于当前设备的MAC地址，则认为不是当前的连接，跳过
        if (settings.isNull() || (!settings->macAddress().isEmpty() && settings->macAddress() != nmDevice->hardwareAddress()))
            continue;

        // 将未保存的连接放入到列表中，供外面调用删除
        if (conn->isUnsaved()) {
            unSaveDevices << conn;
            continue;
        }

        connIsEmpty = true;
    }

    return connIsEmpty;
}

void NetworkModule::addFirstConnection(NetworkManager::WiredDevice *nmDevice)
{
    static bool connectionCreated = false;
    // 只要有一个新增的连接,就不继续新增连接了,因为这个新增的连接是所有网卡共享的
    if (connectionCreated)
        return;

    connectionCreated = true;

    // 先查找当前的设备下是否存在有线连接，如果不存在，则直接新建一个，因为按照要求是至少要有一个有线连接
    NetworkManager::Connection::List unSaveConnections;
    bool findConnection = hasConnection(nmDevice, unSaveConnections);
    // 按照需求，需要将未保存的连接删除
    bool isRemoved = !unSaveConnections.isEmpty();
    for (NetworkManager::Connection::Ptr conn : unSaveConnections)
        conn->remove();

    auto autoCreateConnection = [ & ]() {
        // 如果发现当前的连接的数量为空,则自动创建以当前语言为基础的连接
        ConnectionSettings::Ptr conn(new ConnectionSettings);
        conn->setId(connectionMatchName());
        conn->setUuid("");
        NetworkManager::addConnection(conn->toMap());
    };

    if (!findConnection) {
        if (isRemoved) {
            // 如果有删除的连接，则等待1秒后重新创建
            QTimer::singleShot(1000, this, [ = ] {
                autoCreateConnection();
            });
        } else {
            autoCreateConnection();
        }
    }
}

void NetworkModule::onDeviceStatusChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    if (m_isLockModel) {
        return;
    }
    NetworkManager::Device *device = static_cast<NetworkManager::Device *>(sender());
    NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
    if (!conn.isNull()) {
        m_lastConnection = conn->id();
        m_lastState = newstate;
    } else if(m_lastState != oldstate || m_lastConnection.isEmpty()) {
        m_lastConnection.clear();
        return;
    }
    switch (newstate) {
    case Device::State::Preparing: { // 正在连接
        if (oldstate == Device::State::Disconnected) {
            switch (device->type()) {
            case Device::Type::Ethernet:
                NotificationManager::NetworkNotify(NotificationManager::WiredConnecting, m_lastConnection);
                break;
            case Device::Type::Wifi:
                NotificationManager::NetworkNotify(NotificationManager::WirelessConnecting, m_lastConnection);
                break;
            default: break;
            }
        }
    } break;
    case Device::State::Activated: { // 连接成功
        switch (device->type()) {
        case Device::Type::Ethernet:
            NotificationManager::NetworkNotify(NotificationManager::WiredConnected, m_lastConnection);
            break;
        case Device::Type::Wifi:
            NotificationManager::NetworkNotify(NotificationManager::WirelessConnected, m_lastConnection);
            break;
        default: break;
        }
    } break;
    case Device::State::Failed:
    case Device::State::Disconnected:
    case Device::State::NeedAuth:
    case Device::State::Unmanaged:
    case Device::State::Unavailable: {
        if (reason == Device::StateChangeReason::DeviceRemovedReason) {
            return;
        }

        // ignore if device's old state is not available
        if (oldstate <= Device::State::Unavailable) {
            qDebug("no notify, old state is not available");
            return;
        }

        // ignore invalid reasons
        if (reason == Device::StateChangeReason::UnknownReason) {
            qDebug("no notify, device state reason invalid");
            return;
        }

        switch (reason) {
        case Device::StateChangeReason::UserRequestedReason:
            if (newstate == Device::State::Disconnected) {
                switch (device->type()) {
                case Device::Type::Ethernet:
                    NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, m_lastConnection);
                    break;
                case Device::Type::Wifi:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessDisconnected, m_lastConnection);
                    break;
                default: break;
                }
            }
            break;
        case Device::StateChangeReason::ConfigUnavailableReason:
        case Device::StateChangeReason::AuthSupplicantTimeoutReason: // 超时
            switch (device->type()) {
            case Device::Type::Ethernet:
                NotificationManager::NetworkNotify(NotificationManager::WiredUnableConnect, m_lastConnection);
                break;
            case Device::Type::Wifi:
                NotificationManager::NetworkNotify(NotificationManager::WirelessUnableConnect, m_lastConnection);
                break;
            default: break;
            }
            break;
        case Device::StateChangeReason::AuthSupplicantDisconnectReason:
            if (oldstate == Device::State::ConfiguringHardware && newstate == Device::State::NeedAuth) {
                switch (device->type()) {
                case Device::Type::Ethernet:
                    NotificationManager::NetworkNotify(NotificationManager::WiredConnectionFailed, m_lastConnection);
                    break;
                case Device::Type::Wifi:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessConnectionFailed, m_lastConnection);
                    emit signalShowNetworkDialog();
                    m_networkDialog->setConnectWireless(device->uni(), m_lastConnection);
                    break;
                default: break;
                }
            }
            break;
        case Device::StateChangeReason::CarrierReason:
            if (device->type() == Device::Ethernet) {
                qDebug("unplugged device is ethernet");
                NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, m_lastConnection);
            }
            break;
        case Device::StateChangeReason::NoSecretsReason:
            NotificationManager::NetworkNotify(NotificationManager::NoSecrets, m_lastConnection);
            emit signalShowNetworkDialog();
            m_networkDialog->setConnectWireless(device->uni(), m_lastConnection);
            break;
        case Device::StateChangeReason::SsidNotFound:
            NotificationManager::NetworkNotify(NotificationManager::SsidNotFound, m_lastConnection);
            break;
        default:
            break;
        }
    } break;
    default: break;
    }
}

NetworkPlugin::NetworkPlugin(QObject *parent)
    : QObject(parent)
    , m_network(nullptr)
{
    setObjectName(QStringLiteral(NETWORK_KEY));
}

void NetworkPlugin::init()
{
    initUI();
}

QWidget *NetworkPlugin::content()
{
    return m_network->content();
}

QWidget *NetworkPlugin::itemWidget() const
{
    return m_network->itemWidget();
}

QWidget *NetworkPlugin::itemTipsWidget() const
{
    return m_network->itemTipsWidget();
}

const QString NetworkPlugin::itemContextMenu() const
{
    return m_network->itemContextMenu();
}

void NetworkPlugin::invokedMenuItem(const QString &menuId, const bool checked) const
{
    m_network->invokedMenuItem(menuId, checked);
}

void NetworkPlugin::initUI()
{
    if (m_network) {
        return;
    }
    QTranslator *translator = new QTranslator(this);
    translator->load(QString("/usr/share/dss-network-plugin/translations/dss-network-plugin_%1.qm").arg(QLocale::system().name()));
    QCoreApplication::installTranslator(translator);

    m_network = new NetworkModule(this);
}

} // namespace module
} // namespace dss
