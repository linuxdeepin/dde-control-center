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

#include "connectionhotspoteditpage.h"
#include "settings/hotspotsettings.h"

#include <networkmanagerqt/settings.h>
#include <networkmanagerqt/device.h>
#include <networkmanagerqt/ipv4setting.h>

#include <QDebug>
#include <QVBoxLayout>

using namespace dcc::widgets;
using namespace NetworkManager;

ConnectionHotspotEditPage::ConnectionHotspotEditPage(const QString &devicePath, const QString &connUuid, QWidget *parent)
    : ConnectionEditPage(ConnectionEditPage::ConnectionType::WirelessConnection, devicePath, connUuid, parent, true)
{
}

ConnectionHotspotEditPage::~ConnectionHotspotEditPage()
{
}

void ConnectionHotspotEditPage::initSettingsWidget()
{
    // some special configurations for hotspot
    Ipv4Setting::Ptr ipv4Setting = m_connectionSettings->setting(Setting::SettingType::Ipv4).staticCast<Ipv4Setting>();
    ipv4Setting->setMethod(Ipv4Setting::ConfigMethod::Shared);
    ipv4Setting->setInitialized(true);

    m_settingsWidget = new HotspotSettings(m_connectionSettings, this);
    connect(m_settingsWidget, &HotspotSettings::anyEditClicked, this, [ this ] {
        setButtonTupleEnable(true);
    });

    connect(m_settingsWidget, &AbstractSettings::requestNextPage, this, &ConnectionHotspotEditPage::onRequestNextPage);
    connect(m_settingsWidget, &AbstractSettings::requestFrameAutoHide, this, &ConnectionEditPage::requestFrameAutoHide);

    m_settingsLayout->addWidget(m_settingsWidget);
}
