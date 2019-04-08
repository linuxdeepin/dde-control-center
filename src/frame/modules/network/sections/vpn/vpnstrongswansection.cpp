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

#include "vpnstrongswansection.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnStrongSwanSection::VpnStrongSwanSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent),
      m_vpnSetting(vpnSetting),
      m_gateway(new LineEditWidget(this)),
      m_caCert(new FileChooseWidget(this)),
      m_authTypeChooser(new ComboBoxWidget(this)),

      m_userCert(new FileChooseWidget(this)),
      m_userKey(new FileChooseWidget(this)),
      m_userName(new LineEditWidget(this)),
      m_password(new PasswdEditWidget(this)),

      m_requestInnerIp(new SwitchWidget(this)),
      m_enforceUDP(new SwitchWidget(this)),
      m_useIPComp(new SwitchWidget(this)),
      m_enableCustomCipher(new SwitchWidget(this)),
      m_ike(new LineEditWidget(this)),
      m_esp(new LineEditWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();

    initStrMaps();
    initUI();
    initConnection();

    onAuthTypeChanged(m_currentAuthType);
    onCustomCipherEnableChanged(m_enableCustomCipher->checked());
}

VpnStrongSwanSection::~VpnStrongSwanSection()
{
}

bool VpnStrongSwanSection::allInputValid()
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

void VpnStrongSwanSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    m_dataMap.insert("address", m_gateway->text());
    m_dataMap.insert("certificate", m_caCert->edit()->text());
    m_dataMap.insert("method", m_currentAuthType);

    if (m_currentAuthType == "key" || m_currentAuthType == "agent") {
        m_dataMap.insert("usercert", m_userCert->edit()->text());
        if (m_currentAuthType == "key") {
            m_dataMap.insert("userkey", m_userKey->edit()->text());
        } else {
            m_dataMap.remove("userkey");
        }
    } else {
        m_dataMap.remove("usercert");
        m_dataMap.remove("userkey");
    }

    if (m_currentAuthType == "eap" || m_currentAuthType == "psk") {
        m_dataMap.insert("user", m_userName->text());
        m_secretMap.insert("password", m_password->text());
    } else {
        m_dataMap.remove("user");
        m_secretMap.remove("password");
    }

    if (m_requestInnerIp->checked()) {
        m_dataMap.insert("virtual", "yes");
    } else {
        m_dataMap.remove("virtual");
    }

    if (m_enforceUDP->checked()) {
        m_dataMap.insert("encap", "yes");
    } else {
        m_dataMap.remove("encap");
    }

    if (m_useIPComp->checked()) {
        m_dataMap.insert("ipcomp", "yes");
    } else {
        m_dataMap.remove("ipcomp");
    }

    if (m_enableCustomCipher->checked()) {
        m_dataMap.insert("proposal", "yes");
        m_dataMap.insert("ike", m_ike->text());
        m_dataMap.insert("esp", m_esp->text());
    } else {
        m_dataMap.remove("proposal");
        m_dataMap.remove("esp");
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnStrongSwanSection::initStrMaps()
{
    AuthTypeStrMap = {
        {tr("Private Key"), "key"},
        {tr("SSH Agent"), "agent"},
        {tr("Smartcard"), "smartcard"},
        {tr("EAP"), "eap"},
        {tr("Pre-Shared Key"), "psk"},
    };
}

void VpnStrongSwanSection::initUI()
{
    m_gateway->setTitle(tr("Gateway"));
    m_gateway->setPlaceholderText(tr("Required"));
    m_gateway->setText(m_dataMap.value("address"));

    m_caCert->setTitle(tr("CA Cert"));
    m_caCert->edit()->setText(m_dataMap.value("certificate"));

    m_authTypeChooser->setTitle(tr("Auth Type"));
    m_currentAuthType = "key";
    for (auto value : AuthTypeStrMap.values()) {
        if (value == m_dataMap.value("method")) {
            m_currentAuthType = value;
        }
        m_authTypeChooser->appendOption(AuthTypeStrMap.key(value), value);
    }
    m_authTypeChooser->setCurrent(m_currentAuthType);

    m_userCert->setTitle(tr("User Cert"));
    m_userCert->edit()->setText(m_dataMap.value("usercert"));

    m_userKey->setTitle(tr("Private Key"));
    m_userKey->edit()->setText(m_dataMap.value("userkey"));

    m_userName->setTitle(tr("Username"));
    m_userName->setText(m_dataMap.value("user"));

    m_password->setTitle(tr("Password"));
    m_password->setText(m_secretMap.value("password"));

    m_requestInnerIp->setTitle(tr("Request an Inner IP Address"));
    m_requestInnerIp->setChecked(m_dataMap.value("virtual") == "yes");

    m_enforceUDP->setTitle(tr("Enforce UDP Encapsulation"));
    m_enforceUDP->setChecked(m_dataMap.value("encap") == "yes");

    m_useIPComp->setTitle(tr("Use IP Compression"));
    m_useIPComp->setChecked(m_dataMap.value("ipcomp") == "yes");

    m_enableCustomCipher->setTitle(tr("Enable Custom Cipher Proposals"));
    m_enableCustomCipher->setChecked(m_dataMap.value("proposal") == "yes");

    m_ike->setTitle(tr("IKE"));
    m_ike->setText(m_dataMap.value("ike"));

    m_esp->setTitle(tr("ESP"));
    m_esp->setText(m_dataMap.value("esp"));

    appendItem(m_gateway);
    appendItem(m_caCert);
    appendItem(m_authTypeChooser);

    appendItem(m_userCert);
    appendItem(m_userKey);
    appendItem(m_userName);
    appendItem(m_password);

    appendItem(m_requestInnerIp);
    appendItem(m_enforceUDP);
    appendItem(m_useIPComp);
    appendItem(m_enableCustomCipher);
    appendItem(m_ike);
    appendItem(m_esp);
}

void VpnStrongSwanSection::initConnection()
{
    connect(m_gateway->textEdit(), &QLineEdit::editingFinished, this, &VpnStrongSwanSection::allInputValid);
    connect(m_authTypeChooser, &ComboBoxWidget::requestPage, this, &VpnStrongSwanSection::requestNextPage);
    connect(m_authTypeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onAuthTypeChanged(data.toString());
    });
    connect(m_enableCustomCipher, &SwitchWidget::checkedChanged,
            this, &VpnStrongSwanSection::onCustomCipherEnableChanged);

    connect(m_caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnStrongSwanSection::requestFrameAutoHide);
    connect(m_userCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnStrongSwanSection::requestFrameAutoHide);
    connect(m_userKey, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnStrongSwanSection::requestFrameAutoHide);
}

void VpnStrongSwanSection::onAuthTypeChanged(const QString type)
{
    m_currentAuthType = type;

    m_userCert->setVisible(m_currentAuthType == "key" || m_currentAuthType == "agent");
    m_userKey->setVisible(m_currentAuthType == "key");
    m_userName->setVisible(m_currentAuthType == "eap" || m_currentAuthType == "psk");
    m_password->setVisible(m_currentAuthType == "eap" || m_currentAuthType == "psk");
}

void VpnStrongSwanSection::onCustomCipherEnableChanged(const bool enabled)
{
    m_ike->setVisible(enabled);
    m_esp->setVisible(enabled);
}
