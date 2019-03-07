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

#include "vpnopenconnectsection.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnOpenConnectSection::VpnOpenConnectSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent),
      m_vpnSetting(vpnSetting),
      m_gateway(new LineEditWidget(this)),
      m_caCert(new FileChooseWidget(this)),
      m_proxy(new LineEditWidget(this)),
      m_enableCSDTrojan(new SwitchWidget(this)),
      m_csdScript(new LineEditWidget(this)),
      m_userCert(new FileChooseWidget(this)),
      m_userKey(new FileChooseWidget(this)),
      m_useFSID(new SwitchWidget(this))
{
    m_dataMap = vpnSetting->data();

    initUI();

    connect(m_caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenConnectSection::requestFrameAutoHide);
    connect(m_userCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenConnectSection::requestFrameAutoHide);
    connect(m_userKey, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnOpenConnectSection::requestFrameAutoHide);
}

VpnOpenConnectSection::~VpnOpenConnectSection()
{
}

bool VpnOpenConnectSection::allInputValid()
{
    bool valid = true;
    
    if (m_gateway->text().isEmpty()) {
        valid = false;
        m_gateway->setIsErr(true);
    } else {
        m_gateway->setIsErr(false);
    }

    return valid;
}

void VpnOpenConnectSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    m_dataMap.insert("gateway", m_gateway->text());
    m_dataMap.insert("cacert", m_caCert->edit()->text());
    m_dataMap.insert("proxy", m_proxy->text());
    m_dataMap.insert("enable_csd_trojan", m_enableCSDTrojan->checked() ? "yes" : "no");
    m_dataMap.insert("csd_wrapper", m_csdScript->text());
    m_dataMap.insert("usercert", m_userCert->edit()->text());
    m_dataMap.insert("userkey", m_userKey->edit()->text());
    m_dataMap.insert("pem_passphrase_fsid", m_useFSID->checked() ? "yes" : "no");

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnOpenConnectSection::initUI()
{
    m_gateway->setTitle(tr("Gateway"));
    m_gateway->setPlaceholderText(tr("Required"));
    m_gateway->setText(m_dataMap.value("gateway"));

    m_caCert->setTitle(tr("CA Cert"));
    m_caCert->edit()->setText(m_dataMap.value("cacert"));

    m_proxy->setTitle(tr("Proxy"));
    m_proxy->setText(m_dataMap.value("proxy"));

    m_enableCSDTrojan->setTitle(tr("Allow Cisco Secure Desktop Trojan"));
    m_enableCSDTrojan->setChecked(m_dataMap.value("enable_csd_trojan") == "yes");

    m_csdScript->setTitle(tr("CSD Script"));
    m_csdScript->setText(m_dataMap.value("csd_wrapper"));

    m_userCert->setTitle(tr("User Cert"));
    m_userCert->edit()->setText(m_dataMap.value("usercert"));

    m_userKey->setTitle(tr("Private Key"));
    m_userKey->edit()->setText(m_dataMap.value("userkey"));

    m_useFSID->setTitle(tr("Use FSID for Key Passphrase"));
    m_useFSID->setChecked(m_dataMap.value("pem_passphrase_fsid") == "yes");

    appendItem(m_gateway);
    appendItem(m_caCert);
    appendItem(m_proxy);
    appendItem(m_enableCSDTrojan);
    appendItem(m_csdScript);
    appendItem(m_userCert);
    appendItem(m_userKey);
    appendItem(m_useFSID);
}
