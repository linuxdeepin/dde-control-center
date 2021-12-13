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
    m_networkHelper = new NetworkPluginHelper(this);
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
    QPalette palette = itemTips->palette();
    palette.setColor(QPalette::BrightText, Qt::black);
    itemTips->setPalette(palette);
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
