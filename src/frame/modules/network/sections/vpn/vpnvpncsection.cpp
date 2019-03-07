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

#include "vpnvpncsection.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnVPNCSection::VpnVPNCSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN"), parent),
      m_vpnSetting(vpnSetting),
      m_gateway(new LineEditWidget(this)),
      m_userName(new LineEditWidget(this)),
      m_passwordFlagsChooser(new ComboBoxWidget(this)),
      m_password(new PasswdEditWidget(this)),
      m_groupName(new LineEditWidget(this)),
      m_groupPasswordFlagsChooser(new ComboBoxWidget(this)),
      m_groupPassword(new PasswdEditWidget(this)),
      m_userHybrid(new SwitchWidget(this)),
      m_caFile(new FileChooseWidget(this))
{
    m_dataMap = vpnSetting->data();
    m_secretMap = vpnSetting->secrets();

    initStrMaps();

    m_currentPasswordType = (NetworkManager::Setting::SecretFlagType)m_dataMap
        .value("Xauth password-flags", "0").toInt();
    m_currentGroupPasswordType = (NetworkManager::Setting::SecretFlagType)m_dataMap
        .value("IPSec secret-flags", "0").toInt();

    initUI();
    initConnection();

    onPasswordFlagsChanged(m_currentPasswordType);
    onGroupPasswordFlagsChanged(m_currentGroupPasswordType);
}

VpnVPNCSection::~VpnVPNCSection()
{
}

bool VpnVPNCSection::allInputValid()
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
    if (m_groupName->text().isEmpty()) {
        valid = false;
        m_groupName->setIsErr(true);
    } else {
        m_groupName->setIsErr(false);
    }
    if (m_currentGroupPasswordType == NetworkManager::Setting::SecretFlagType::None
            && m_groupPassword->text().isEmpty()) {
        valid = false;
        m_groupPassword->setIsErr(true);
    } else {
        m_groupPassword->setIsErr(false);
    }
    if (m_userHybrid->checked() && m_caFile->edit()->text().isEmpty()) {
        valid = false;
        m_caFile->setIsErr(true);
    } else {
        m_caFile->setIsErr(false);
    }

    return valid;
}

void VpnVPNCSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();
    m_secretMap = m_vpnSetting->secrets();

    // save values about user
    m_dataMap.insert("IPSec gateway", m_gateway->text());
    m_dataMap.insert("Xauth username", m_userName->text());
    m_dataMap.insert("Xauth password-flags", QString::number(m_currentPasswordType));
    if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::None) {
        m_dataMap.insert("xauth-password-type", "save");
        m_secretMap.insert("Xauth password", m_password->text());
    } else if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::NotSaved) {
        m_dataMap.insert("xauth-password-type", "ask");
        m_secretMap.remove("Xauth password");
    } else if (m_currentPasswordType == NetworkManager::Setting::SecretFlagType::NotRequired) {
        m_dataMap.insert("xauth-password-type", "unused");
        m_secretMap.remove("Xauth password");
    }
    // save values about gourp
    m_dataMap.insert("IPSec ID", m_groupName->text());
    m_dataMap.insert("IPSec secret-flags", QString::number(m_currentGroupPasswordType));
    if (m_currentGroupPasswordType == NetworkManager::Setting::SecretFlagType::None) {
        m_dataMap.insert("ipsec-secret-type", "save");
        m_secretMap.insert("IPSec secret", m_groupPassword->text());
    } else if (m_currentGroupPasswordType == NetworkManager::Setting::SecretFlagType::NotSaved) {
        m_dataMap.insert("ipsec-secret-type", "ask");
        m_secretMap.remove("IPSec secret");
    } else if (m_currentGroupPasswordType == NetworkManager::Setting::SecretFlagType::NotRequired) {
        m_dataMap.insert("ipsec-secret-type", "unused");
        m_secretMap.remove("IPSec secret");
    }

    if (m_userHybrid->checked()) {
        m_dataMap.insert("IKE Authmode", "hybrid");
        m_dataMap.insert("CA-File", m_caFile->edit()->text());
    } else {
        m_dataMap.remove("IKE Authmode");
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setSecrets(m_secretMap);

    m_vpnSetting->setInitialized(true);
}

void VpnVPNCSection::initStrMaps()
{
    PasswordFlagsStrMap = {
        //{"Saved", NetworkManager::Setting::AgentOwned},
        {tr("Saved"), NetworkManager::Setting::SecretFlagType::None},
        {tr("Ask"), NetworkManager::Setting::SecretFlagType::NotSaved},
        {tr("Not Required"), NetworkManager::Setting::SecretFlagType::NotRequired}
    };
}

void VpnVPNCSection::initUI()
{
    m_gateway->setTitle(tr("Gateway"));
    m_gateway->setPlaceholderText(tr("Required"));
    m_gateway->setText(m_dataMap.value("IPSec gateway"));

    m_userName->setTitle(tr("Username"));
    m_userName->setPlaceholderText(tr("Required"));
    m_userName->setText(m_dataMap.value("Xauth username"));

    m_passwordFlagsChooser->setTitle(tr("Pwd Options"));
    for (const QString &key : PasswordFlagsStrMap.keys()) {
        m_passwordFlagsChooser->appendOption(key, PasswordFlagsStrMap.value(key));
    }
    m_passwordFlagsChooser->setCurrent(m_currentPasswordType);

    m_password->setTitle(tr("Password"));
    m_password->setPlaceholderText(tr("Required"));
    m_password->setText(m_secretMap.value("Xauth password"));

    m_groupName->setTitle(tr("Group Name"));
    m_groupName->setPlaceholderText(tr("Required"));
    m_groupName->setText(m_dataMap.value("IPSec ID"));

    m_groupPasswordFlagsChooser->setTitle(tr("Pwd Options"));
    for (const QString &key : PasswordFlagsStrMap.keys()) {
        m_groupPasswordFlagsChooser->appendOption(key, PasswordFlagsStrMap.value(key));
    }
    m_groupPasswordFlagsChooser->setCurrent(m_currentGroupPasswordType);

    m_groupPassword->setTitle(tr("Group Pwd"));
    m_groupPassword->setPlaceholderText(tr("Required"));
    m_groupPassword->setText(m_secretMap.value("IPSec secret"));

    m_userHybrid->setTitle(tr("Use Hybrid Authentication"));
    m_userHybrid->setChecked(m_dataMap.value("IKE Authmode") == "hybrid");

    m_caFile->setTitle(tr("CA File"));
    m_caFile->edit()->setText(m_dataMap.value("CA-File"));
    m_caFile->setVisible(m_userHybrid->checked());

    appendItem(m_gateway);
    appendItem(m_userName);
    appendItem(m_passwordFlagsChooser);
    appendItem(m_password);
    appendItem(m_groupName);
    appendItem(m_groupPasswordFlagsChooser);
    appendItem(m_groupPassword);
    appendItem(m_userHybrid);
    appendItem(m_caFile);
}

void VpnVPNCSection::initConnection()
{
    connect(m_gateway->textEdit(), &QLineEdit::editingFinished, this, &VpnVPNCSection::allInputValid);
    connect(m_userName->textEdit(), &QLineEdit::editingFinished, this, &VpnVPNCSection::allInputValid);
    connect(m_password->textEdit(), &QLineEdit::editingFinished, this, &VpnVPNCSection::allInputValid);
    connect(m_passwordFlagsChooser, &ComboBoxWidget::requestPage, this, &VpnVPNCSection::requestNextPage);
    connect(m_passwordFlagsChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onPasswordFlagsChanged(data.value<NetworkManager::Setting::SecretFlagType>());
    });
    connect(m_groupName->textEdit(), &QLineEdit::editingFinished, this, &VpnVPNCSection::allInputValid);
    connect(m_groupPasswordFlagsChooser, &ComboBoxWidget::requestPage, this, &VpnVPNCSection::requestNextPage);
    connect(m_groupPasswordFlagsChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onGroupPasswordFlagsChanged(data.value<NetworkManager::Setting::SecretFlagType>());
    });
    connect(m_groupPassword->textEdit(), &QLineEdit::editingFinished, this, &VpnVPNCSection::allInputValid);
    connect(m_userHybrid, &SwitchWidget::checkedChanged, this, [=](const bool checked) {
        m_caFile->setVisible(checked);
        allInputValid();
    });

    connect(m_caFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnVPNCSection::requestFrameAutoHide);
}

void VpnVPNCSection::onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type)
{
    m_currentPasswordType = type;
    m_password->setVisible(m_currentPasswordType ==NetworkManager::Setting::SecretFlagType::None);
}

void VpnVPNCSection::onGroupPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type)
{
    m_currentGroupPasswordType = type;
    m_groupPassword->setVisible(m_currentGroupPasswordType ==NetworkManager::Setting::SecretFlagType::None);
}
