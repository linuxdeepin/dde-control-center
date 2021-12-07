/*
* Copyright (C) 2021 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "trayicon.h"
#include "networkpluginhelper.h"
#include "dde-dock/constants.h"
#include "imageutil.h"

#include <QMouseEvent>
#include <QPainter>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <wirelessdevice.h>

NETWORKPLUGIN_USE_NAMESPACE

TrayIcon::TrayIcon(NetworkPluginHelper *networkHelper)
    : QWidget()
    , m_networkHelper(networkHelper)
    , m_greeterStyle(false)
    , m_refreshIconTimer(new QTimer(this))
{
    m_networkHelper->setMainWidget(this);
    setAccessibleName(QStringLiteral("NetworkTrayIcon"));
    setFixedSize(QSize(20, 20));
    setBackgroundRole(DPalette::Button);

    m_refreshIconTimer->setInterval(100);
    connect(m_refreshIconTimer, &QTimer::timeout, this, &TrayIcon::refreshIcon);
    connect(m_networkHelper, &NetworkPluginHelper::viewUpdate, this, &TrayIcon::refreshIcon);
    connect(Dtk::Gui::DGuiApplicationHelper::instance(), &Dtk::Gui::DGuiApplicationHelper::themeTypeChanged, this, &TrayIcon::refreshIcon);
}

void TrayIcon::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);

    QPainter painter(this);
    const QRectF &rf = rect();
    const QRectF &rfp = QRectF(m_iconPixmap.rect());
    if (m_greeterStyle) { // 锁屏按钮有0.7透明度
        painter.setOpacity(0.70);
    }
    painter.drawPixmap(rf.center() - rfp.center() / m_iconPixmap.devicePixelRatioF(),
                       m_iconPixmap);
}

void TrayIcon::resizeEvent(QResizeEvent *e)
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

void TrayIcon::showNetworkDialog()
{
    if (isVisible()) {
        emit signalShowNetworkDialog(this);
    }
}

QString TrayIcon::getStrengthStateString(int strength) const
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

int TrayIcon::getStrongestAp()
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

bool TrayIcon::isDarkIcon() const
{
    // 如果是登陆或者锁屏界面，始终显示白色图标
    if (m_greeterStyle)
        return false;

    // 如果当前是白色主题，则任务栏的尺寸是最小尺寸，则显示黑色图标
    if (DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::LightType) {
        if (height() <= PLUGIN_BACKGROUND_MIN_SIZE)
            return true;
    }

    return false;
}

void TrayIcon::refreshIcon()
{
    QString stateString;
    QString iconString;
    const auto ratio = devicePixelRatioF();
    int iconSize = PLUGIN_ICON_MAX_SIZE;
    int strength = 0;

    bool useDarkIcon = isDarkIcon();

    switch (m_networkHelper->getPluginState()) {
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
        if (QTime::currentTime().second() & 2) {
            strength = QTime::currentTime().msec() / 10 % 100;
            stateString = getStrengthStateString(strength);
            iconString = QString("wireless-%1-symbolic").arg(stateString);
            if (useDarkIcon)
                iconString.append(PLUGIN_MIN_ICON_NAME);
            m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);
            update();
            return;
        } else {
            m_refreshIconTimer->start(200);
            const int index = QTime::currentTime().msec() / 200 % 10;
            const int num = index + 1;
            iconString = QString("network-wired-symbolic-connecting%1").arg(num);
            if (useDarkIcon)
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
        if (useDarkIcon)
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
        if (useDarkIcon)
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
    case PluginState::WirelessIpConflicted: {
        stateString = "offline";
        iconString = QString("network-wireless-%1-symbolic").arg(stateString);
        break;
    }
    case PluginState::WiredIpConflicted: {
        stateString = "offline";
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

    m_refreshIconTimer->stop();

    if (useDarkIcon)
        iconString.append(PLUGIN_MIN_ICON_NAME);

    m_iconPixmap = ImageUtil::loadSvg(iconString, ":/", iconSize, ratio);

    update();
}

void TrayIcon::setGreeterStyle(bool greeterStyle)
{
    m_greeterStyle = greeterStyle;
}
