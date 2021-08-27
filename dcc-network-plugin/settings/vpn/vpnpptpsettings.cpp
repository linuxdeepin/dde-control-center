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

#include "vpnpptpsettings.h"
#include "../../sections/genericsection.h"
#include "../../sections/vpn/vpnsection.h"
#include "../../sections/vpn/vpnpppsection.h"
#include "../../sections/vpn/vpnipsecsection.h"
#include "../../sections/ipvxsection.h"
#include "../../sections/dnssection.h"

#include <QVBoxLayout>

#include <widgets/contentwidget.h>

using namespace NetworkManager;

VpnPPTPSettings::VpnPPTPSettings(ConnectionSettings::Ptr connSettings, QWidget *parent)
    : AbstractSettings(connSettings, parent)
{
    initSections();
}

VpnPPTPSettings::~VpnPPTPSettings()
{
}

void VpnPPTPSettings::initSections()
{
    VpnSetting::Ptr vpnSetting = m_connSettings->setting(Setting::SettingType::Vpn).staticCast<VpnSetting>();

    if (!vpnSetting)
        return;

    GenericSection *genericSection = new GenericSection(m_connSettings);
    genericSection->setConnectionType(ConnectionSettings::Vpn);

    VpnSection *vpnSection = new VpnSection(vpnSetting);
    VpnPPPSection *vpnPPPSection = new VpnPPPSection(vpnSetting);
    QStringList supportOptions = {
        "refuse-eap", "refuse-pap", "refuse-chap", "refuse-mschap", "refuse-mschapv2",
        "nobsdcomp", "nodeflate", "no-vj-comp", "lcp-echo-interval"
    };

    vpnPPPSection->setSupportOptions(supportOptions);
    IpvxSection *ipv4Section = new IpvxSection(m_connSettings->setting(Setting::SettingType::Ipv4).staticCast<Ipv4Setting>());
    ipv4Section->setIpv4ConfigMethodEnable(Ipv4Setting::ConfigMethod::Manual, false);
    ipv4Section->setNeverDefaultEnable(true);
    DNSSection *dnsSection = new DNSSection(m_connSettings, false);

    connect(genericSection, &GenericSection::editClicked, this, &VpnPPTPSettings::anyEditClicked);
    connect(vpnSection, &VpnSection::editClicked, this, &VpnPPTPSettings::anyEditClicked);
    connect(vpnPPPSection, &VpnPPPSection::editClicked, this, &VpnPPTPSettings::anyEditClicked);
    connect(ipv4Section, &IpvxSection::editClicked, this, &VpnPPTPSettings::anyEditClicked);
    connect(dnsSection, &DNSSection::editClicked, this, &VpnPPTPSettings::anyEditClicked);

    connect(vpnSection, &VpnSection::requestNextPage, this, &VpnPPTPSettings::requestNextPage);
    connect(vpnPPPSection, &VpnPPPSection::requestNextPage, this, &VpnPPTPSettings::requestNextPage);
    connect(ipv4Section, &IpvxSection::requestNextPage, this, &VpnPPTPSettings::requestNextPage);
    connect(dnsSection, &DNSSection::requestNextPage, this, &VpnPPTPSettings::requestNextPage);

    connect(vpnSection, &VpnSection::requestFrameAutoHide, this, &VpnPPTPSettings::requestFrameAutoHide);
    connect(vpnPPPSection, &VpnPPPSection::requestFrameAutoHide, this, &VpnPPTPSettings::requestFrameAutoHide);
    connect(ipv4Section, &IpvxSection::requestFrameAutoHide, this, &VpnPPTPSettings::requestFrameAutoHide);
    connect(dnsSection, &DNSSection::requestFrameAutoHide, this, &VpnPPTPSettings::requestFrameAutoHide);

    m_sectionsLayout->addWidget(genericSection);
    m_sectionsLayout->addWidget(vpnSection);
    m_sectionsLayout->addWidget(vpnPPPSection);
    m_sectionsLayout->addWidget(ipv4Section);
    m_sectionsLayout->addWidget(dnsSection);

    m_settingSections.append(genericSection);
    m_settingSections.append(vpnSection);
    m_settingSections.append(vpnPPPSection);
    m_settingSections.append(ipv4Section);
    m_settingSections.append(dnsSection);
}
