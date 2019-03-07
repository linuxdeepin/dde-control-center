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

#include "connectionwirelesseditpage.h"
#include "settings/wirelesssettings.h"

#include <networkmanagerqt/wirelesssetting.h>
#include <networkmanagerqt/wirelesssecuritysetting.h>

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionWirelessEditPage::ConnectionWirelessEditPage(const QString &devPath, const QString &connUuid, QWidget *parent)
    : ConnectionEditPage(ConnectionEditPage::ConnectionType::WirelessConnection, devPath, connUuid, parent)
{
}

ConnectionWirelessEditPage::~ConnectionWirelessEditPage()
{
}

void ConnectionWirelessEditPage::initSettingsWidgetFromAp(const QString &apPath)
{
    qDebug() << "create wireless connection from ap:" << apPath;

    NetworkManager::AccessPoint::Ptr nmAp = QSharedPointer<AccessPoint>(new NetworkManager::AccessPoint(apPath));

    if (!m_connectionSettings) {
        qDebug() << "ConnectionSettings of base class is invalid..." << m_connectionSettings;
        return;
    }

    if (!nmAp) {
        qDebug() << "AccessPoint path is invalid..." << apPath;
        return;
    }

    // init connection setting from data of given ap
    m_connectionSettings->setId(nmAp->ssid());

    initApSecretType(nmAp);

    m_connectionSettings->setting(NetworkManager::Setting::SettingType::Wireless)
        .staticCast<NetworkManager::WirelessSetting>()->setSsid(nmAp->rawSsid());

    m_settingsWidget = new WirelessSettings(m_connectionSettings, this);

    connect(m_settingsWidget, &WirelessSettings::requestNextPage, this, &ConnectionWirelessEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);
    m_settingsLayout->addWidget(m_settingsWidget);
}

void ConnectionWirelessEditPage::initApSecretType(NetworkManager::AccessPoint::Ptr nmAp)
{
    NetworkManager::WirelessSecuritySetting::Ptr wirelessSecuritySetting;
    wirelessSecuritySetting = m_connectionSettings->setting(NetworkManager::Setting::SettingType::WirelessSecurity)
        .staticCast<NetworkManager::WirelessSecuritySetting>();

    NetworkManager::AccessPoint::Capabilities capabilities = nmAp->capabilities();
    NetworkManager::AccessPoint::WpaFlags wpaFlags = nmAp->wpaFlags();
    NetworkManager::AccessPoint::WpaFlags rsnFlags = nmAp->rsnFlags();

    NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone;

    if (capabilities.testFlag(NetworkManager::AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep;
    }

    if (wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmtPsk)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk;
    }

    if (wpaFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(NetworkManager::AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap;
    }

    wirelessSecuritySetting->setKeyMgmt(keyMgmt);
}
