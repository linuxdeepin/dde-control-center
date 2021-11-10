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
#include "networkpanel.h"
#include "networkdialog.h"
#include "trayicon.h"
#include "notificationmanager.h"

#include <QWidget>

#include <networkcontroller.h>

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WiredDevice>

#define NETWORK_KEY "network-item-key"

namespace dss {
namespace module {

NetworkModule::NetworkModule(QObject *parent)
    : QObject(parent)
{
    QDBusConnection::sessionBus().connect("com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront", "Visible", this, SLOT(updateLockScreenStatus(bool)));
    m_isLockModel = QDBusConnection::sessionBus().interface()->isServiceRegistered("com.deepin.dde.lockFront");
    if (!m_isLockModel) {
        dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
    }

    m_networkDialog = new NetworkDialog(this);
    m_networkDialog->setRunReason(NetworkDialog::Lock);
    m_networkPanel = new NetworkPanel(this);
    m_networkPanel->setGreeterStyle(true);
    if (!m_isLockModel) {
        m_networkDialog->setRunReason(NetworkDialog::Greeter);
        connect(m_networkPanel, &NetworkPanel::addDevice, this, &NetworkModule::onAddDevice);
    }
}

QWidget *NetworkModule::itemWidget() const
{
    TrayIcon *trayIcon = new TrayIcon(m_networkPanel);
    connect(this, &NetworkModule::signalShowNetworkDialog, trayIcon, &TrayIcon::showNetworkDialog);
    connect(trayIcon, &TrayIcon::signalShowNetworkDialog, this, &NetworkModule::showNetworkDialog);
    // 处理内存
    connect(m_networkPanel, &NetworkPanel::destroyed, trayIcon, &TrayIcon::deleteLater);
    return trayIcon;
}

QWidget *NetworkModule::itemTipsWidget() const
{
    return m_networkPanel->itemTips();
}

const QString NetworkModule::itemContextMenu() const
{
    return m_networkPanel->contextMenu(false);
}

void NetworkModule::invokedMenuItem(const QString &menuId, const bool checked) const
{
    Q_UNUSED(checked);
    m_networkPanel->invokeMenuItem(menuId);
}

void NetworkModule::showNetworkDialog(QWidget *w) const
{
    QPoint point = w->mapToGlobal(QPoint(w->width() / 2, 0));
    m_networkDialog->show(point.x(), point.y(), Dock::Position::Bottom);
}

void NetworkModule::updateLockScreenStatus(bool lock)
{
    m_isLockModel = true;
    m_isLockScreen = lock;
}

void NetworkModule::onAddDevice(const QString &devicePath)
{
    if (m_isLockModel) {
        return;
    }
    // 登录界面才监听该信号，用于横幅、密码错误处理
    if (!m_devicePaths.contains(devicePath)) {
        NetworkManager::Device::Ptr device(new NetworkManager::Device(devicePath));
        NetworkManager::Device *nmDevice = nullptr;
        if (device->type() == NetworkManager::Device::Wifi) {
            NetworkManager::WirelessDevice *wDevice = new NetworkManager::WirelessDevice(devicePath, this);
            nmDevice = wDevice;
            connect(wDevice, &NetworkManager::WirelessDevice::activeAccessPointChanged, this, [this](const QString &ap) {
                m_lastActiveWirelessDevicePath = static_cast<NetworkManager::WirelessDevice *>(sender())->uni() + ap;
            });
        } else {
            nmDevice = new NetworkManager::Device(devicePath, this);
        }
        connect(nmDevice, &NetworkManager::Device::stateChanged, this, &NetworkModule::onDeviceStatusChanged);
        m_devicePaths.insert(devicePath);
    }
}

void NetworkModule::onDeviceStatusChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    if (m_isLockModel) {
        return;
    }
    NetworkManager::Device *d = static_cast<NetworkManager::Device *>(sender());
    NetworkManager::Device::Ptr device(new NetworkManager::Device(d->uni()));
    switch (newstate) {
    case NetworkManager::Device::State::Preparing: { // 正在连接
        NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
        if (oldstate == NetworkManager::Device::State::Disconnected && !conn.isNull()) {
            switch (device->type()) {
            case NetworkManager::Device::Type::Ethernet:
                NotificationManager::NetworkNotify(NotificationManager::WiredConnecting, conn->id());
                break;
            case NetworkManager::Device::Type::Wifi:
                NotificationManager::NetworkNotify(NotificationManager::WirelessConnecting, conn->id());
                break;
            }
        }
    } break;
    case NetworkManager::Device::State::Activated: { // 连接成功
        NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
        if (conn.isNull()) {
            return;
        }
        switch (device->type()) {
        case NetworkManager::Device::Type::Ethernet:
            NotificationManager::NetworkNotify(NotificationManager::WiredConnected, conn->id());
            break;
        case NetworkManager::Device::Type::Wifi:
            NotificationManager::NetworkNotify(NotificationManager::WirelessConnected, conn->id());
            break;
        }
    } break;
    case NetworkManager::Device::State::Failed: {
        if (device->type() == NetworkManager::Device::Wifi) {
            switch (reason) {
            case NetworkManager::Device::NoSecretsReason:
            case NetworkManager::Device::ConfigUnavailableReason:
                m_networkDialog->setConnectWireless(m_lastActiveWirelessDevicePath);
                emit signalShowNetworkDialog();
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

        // ignore invalid reasons
        if (reason == NetworkManager::Device::StateChangeReason::UnknownReason) {
            qDebug("no notify, device state reason invalid");
            return;
        }

        switch (reason) {
        case NetworkManager::Device::StateChangeReason::UserRequestedReason:
            if (newstate == NetworkManager::Device::State::Disconnected) {
                switch (device->type()) {
                case NetworkManager::Device::Type::Ethernet:
                    NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, conn->id());
                    break;
                case NetworkManager::Device::Type::Wifi:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessDisconnected, conn->id());
                    break;
                }
            }
            break;
        case NetworkManager::Device::StateChangeReason::NewActivation:
        case NetworkManager::Device::StateChangeReason::ConfigUnavailableReason:
            switch (conn->type()) {
            case NetworkManager::ConnectionSettings::ConnectionType::Wired:
                NotificationManager::NetworkNotify(NotificationManager::WiredUnableConnect, conn->id());
                break;
            case NetworkManager::ConnectionSettings::ConnectionType::Wireless:
                NotificationManager::NetworkNotify(NotificationManager::WirelessUnableConnect, conn->id());
                break;
            }
            break;
        case NetworkManager::Device::StateChangeReason::AuthSupplicantDisconnectReason:
            if (oldstate == NetworkManager::Device::State::ConfiguringHardware && newstate == NetworkManager::Device::State::NeedAuth) {
                switch (conn->type()) {
                case NetworkManager::ConnectionSettings::ConnectionType::Wired:
                    NotificationManager::NetworkNotify(NotificationManager::WiredConnectionFailed, conn->id());
                    break;
                case NetworkManager::ConnectionSettings::ConnectionType::Wireless:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessConnectionFailed, conn->id());
                    break;
                }
            }
            break;
        case NetworkManager::Device::StateChangeReason::CarrierReason:
            if (device->type() == NetworkManager::Device::Ethernet) {
                qDebug("unplugged device is ethernet");
                NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, conn->id());
            }
            break;
        case NetworkManager::Device::StateChangeReason::NoSecretsReason:
            NotificationManager::NetworkNotify(NotificationManager::NoSecrets, conn->id());
            break;
        case NetworkManager::Device::StateChangeReason::SsidNotFound:
            NotificationManager::NetworkNotify(NotificationManager::SsidNotFound, conn->id());
            break;
        }
    } break;
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
    return nullptr;
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
