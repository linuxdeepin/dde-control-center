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

#include "vpnsection.h"

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnSection::VpnSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent)
    , m_vpnSetting(vpnSetting)
    , m_gateway(new LineEditWidget(this))
    , m_userName(new LineEditWidget(this))
    , m_passwordFlagsChooser(new ComboxWidget(this))
    , m_password(new LineEditWidget(true, this))
    , m_domain(new LineEditWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();
    m_currentPasswordType = (NetworkManager::Setting::SecretFlagType)m_dataMap.value("password-flags", "0").toInt();

    initStrMaps();
    initUI();
    initConnection();

    onPasswordFlagsChanged(m_currentPasswordType);
}

VpnSection::~VpnSection()
{
}

bool VpnSection::allInputValid()
{
    bool valid = true;

    if (m_gateway->text().isEmpty()) {
        valid = false;
        m_gateway->setIsErr(true);
    } else {
        m_gateway->setIsErr(false);
    }
    if (m_userName->text().isEmpty()) {
        valid = false;
        m_userName->setIsErr(true);
    } else {
        m_userName->setIsErr(false);
    }
    if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None
            && m_password->text().isEmpty()) {
        valid = false;
        m_password->setIsErr(true);
    } else {
        m_password->setIsErr(false);
    }

    return valid;
}

void VpnSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    m_dataMap.insert("gateway", m_gateway->text());
    m_dataMap.insert("user", m_userName->text());
    m_dataMap.insert("password-flags", QString::number(m_currentPasswordType));
    if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None) {
        m_secretMap.insert("password", m_password->text());
    } else {
        m_secretMap.remove("password");
    }
    m_dataMap.insert("domain", m_domain->text());

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnSection::initStrMaps()
{
    PasswordFlagsStrMap = {
        //{tr("Saved"), NetworkManager::Setting::AgentOwned},
        {tr("Saved"), NetworkManager::Setting::SecretFlagType::None},
        {tr("Ask"), NetworkManager::Setting::SecretFlagType::NotSaved},
        {tr("Not Required"), NetworkManager::Setting::SecretFlagType::NotRequired}
    };
}

void VpnSection::initUI()
{
    m_gateway->setTitle(tr("Gateway"));
    m_gateway->setPlaceholderText(tr("Required"));
    m_gateway->setText(m_dataMap.value("gateway"));

    m_userName->setTitle(tr("Username"));
    m_userName->setPlaceholderText(tr("Required"));
    m_userName->setText(m_dataMap.value("user"));

    m_passwordFlagsChooser->setTitle(tr("Pwd Options"));
    QStringList comboxOptions;
    QString curOption;
    for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
        comboxOptions << it->first;
        if (it->second == m_currentPasswordType) {
            curOption = it->first;
        }
    }

    m_passwordFlagsChooser->setComboxOption(comboxOptions);
    m_passwordFlagsChooser->setCurrentText(curOption);

    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Required"));
    m_password->setText(m_secretMap.value("password"));
    m_password->setForbiddenChineseInput();

    m_domain->setTitle(tr("NT Domain"));
    m_domain->setText(m_dataMap.value("domain"));

    appendItem(m_gateway);
    appendItem(m_userName);
    appendItem(m_passwordFlagsChooser);
    appendItem(m_password);
    appendItem(m_domain);
}

void VpnSection::initConnection()
{
    connect(m_passwordFlagsChooser, &ComboxWidget::onSelectChanged, [this](const QString &passwordKey) {
        for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
            if (it->first == passwordKey) {
                onPasswordFlagsChanged(it->second);
                break;
            }
        }
    });
}

void VpnSection::onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type)
{
    m_currentPasswordType = type;
    m_password->setVisible(m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None);
}
