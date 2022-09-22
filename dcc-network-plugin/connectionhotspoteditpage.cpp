// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
