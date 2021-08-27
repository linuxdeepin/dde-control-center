/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionWirelessEditPage::ConnectionWirelessEditPage(const QString &devPath, const QString &connUuid, bool isHidden, QWidget *parent)
    : ConnectionEditPage(ConnectionEditPage::ConnectionType::WirelessConnection, devPath, connUuid, parent)
{
    if (isHidden)
        m_connectionSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>()->setHidden(isHidden);
}

ConnectionWirelessEditPage::~ConnectionWirelessEditPage()
{
}

void ConnectionWirelessEditPage::initSettingsWidgetFromAp(const QString &apPath)
{
    AccessPoint::Ptr nmAp = QSharedPointer<AccessPoint>(new AccessPoint(apPath));

    if (!m_connectionSettings || !nmAp)
        return;

    // init connection setting from data of given ap
    m_connectionSettings->setId(nmAp->ssid());

    initApSecretType(nmAp);

    m_connectionSettings->setting(Setting::SettingType::Wireless).staticCast<WirelessSetting>()->setSsid(nmAp->rawSsid());

    m_settingsWidget = new WirelessSettings(m_connectionSettings, this);

    connect(m_settingsWidget, &WirelessSettings::requestNextPage, this, &ConnectionWirelessEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);
    connect(m_settingsWidget, &WirelessSettings::anyEditClicked, this, [ this ] {
        setButtonTupleEnable(true);
    });

    m_settingsLayout->addWidget(m_settingsWidget);
}

void ConnectionWirelessEditPage::initApSecretType(AccessPoint::Ptr nmAp)
{
    WirelessSecuritySetting::Ptr wirelessSecuritySetting;
    wirelessSecuritySetting = m_connectionSettings->setting(Setting::SettingType::WirelessSecurity).staticCast<WirelessSecuritySetting>();

    AccessPoint::Capabilities capabilities = nmAp->capabilities();
    AccessPoint::WpaFlags wpaFlags = nmAp->wpaFlags();
    AccessPoint::WpaFlags rsnFlags = nmAp->rsnFlags();

    WirelessSecuritySetting::KeyMgmt keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaNone;

    if (capabilities.testFlag(AccessPoint::Capability::Privacy) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) &&
            !wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::Wep;
    }

    if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmtPsk)) {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaPsk;
    }

    if (wpaFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x) ||
            rsnFlags.testFlag(AccessPoint::WpaFlag::KeyMgmt8021x)) {
        keyMgmt = WirelessSecuritySetting::KeyMgmt::WpaEap;
    }

    wirelessSecuritySetting->setKeyMgmt(keyMgmt);
}
