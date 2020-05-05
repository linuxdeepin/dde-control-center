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

#include "secretwirelesssection.h"
#include "widgets/contentwidget.h"

#include <networkmanagerqt/utils.h>

#include <QComboBox>

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;

SecretWirelessSection::SecretWirelessSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting,
                                             NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent)
    : Secret8021xSection(sSetting, parent)
    , m_keyMgmtChooser(new ComboxWidget(this))
    , m_passwdEdit(new LineEditWidget(true, this))
    , m_enableWatcher(new Secret8021xEnableWatcher(this))
    , m_authAlgChooser(new ComboxWidget(this))
    , m_currentKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone)
    , m_currentAuthAlg(NetworkManager::WirelessSecuritySetting::AuthAlg::Shared)
    , m_wsSetting(wsSeting)
    , m_s8Setting(sSetting)
{
    initStrMaps();

    // init KeyMgmt
    const NetworkManager::WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    m_currentKeyMgmt = (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) ?
                       NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;
    m_currentAuthAlg = (m_wsSetting->authAlg() == NetworkManager::WirelessSecuritySetting::AuthAlg::Open) ?
                        m_wsSetting->authAlg() : NetworkManager::WirelessSecuritySetting::AuthAlg::Shared;

    initUI();
    initConnection();

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretWirelessSection::~SecretWirelessSection()
{
}

bool SecretWirelessSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        valid = NetworkManager::wepKeyIsValid(m_passwdEdit->text(),
                                              NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
        m_passwdEdit->setIsErr(!valid);
        if (!valid && !m_passwdEdit->text().isEmpty()) {
            m_passwdEdit->showAlertMessage(tr("Invalid password"));
        }
    }

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        valid = NetworkManager::wpaPskIsValid(m_passwdEdit->text());
        m_passwdEdit->setIsErr(!valid);
        if (!valid && !m_passwdEdit->text().isEmpty()) {
            m_passwdEdit->showAlertMessage(tr("Invalid password"));
        }
    }

    return valid ? Secret8021xSection::allInputValid() : false;
}

void SecretWirelessSection::saveSettings()
{
    m_wsSetting->setKeyMgmt(m_currentKeyMgmt);

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone
            || m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) {
        return m_wsSetting->setInitialized(false);
    }

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        m_wsSetting->setWepKeyType(NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
        m_wsSetting->setWepKey0(m_passwdEdit->text());
        m_wsSetting->setAuthAlg(m_currentAuthAlg);
    } else if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        m_wsSetting->setPsk(m_passwdEdit->text());
    }

    m_wsSetting->setInitialized(true);

    Secret8021xSection::saveSettings();
}

void SecretWirelessSection::initStrMaps()
{
    KeyMgmtStrMap = {
        {tr("None"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone},
        {tr("WEP 40/64/128-bit"), NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep},
        {tr("WPA/WPA2 Personal"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk},
        {tr("WPA/WPA2 Enterprise"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap}
    };

    AuthAlgStrMap = {
        {tr("Shared key"), NetworkManager::WirelessSecuritySetting::AuthAlg::Shared},
        {tr("Open system"), NetworkManager::WirelessSecuritySetting::AuthAlg::Open}
    };
}

void SecretWirelessSection::initUI()
{
    m_keyMgmtChooser->setTitle(tr("Security"));
    QString curMgmtOption = KeyMgmtStrMap.at(0).first;
    for (auto it = KeyMgmtStrMap.cbegin(); it != KeyMgmtStrMap.cend(); ++it) {
        m_keyMgmtChooser->comboBox()->addItem(it->first, it->second);
        if (m_currentKeyMgmt == it->second) {
            curMgmtOption = it->first;
        }
    }
    m_keyMgmtChooser->setCurrentText(curMgmtOption);

    m_passwdEdit->setPlaceholderText(tr("Required"));

    m_enableWatcher->setSecretEnable(m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap);

    QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodLeap);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodPeap);

    init(m_enableWatcher, eapMethodsWantedList);

    m_authAlgChooser->setTitle(tr("Authentication"));
    QString curAuthAlgOption = AuthAlgStrMap.at(0).first;
    for (auto it = AuthAlgStrMap.cbegin(); it != AuthAlgStrMap.cend(); ++it) {
        m_authAlgChooser->comboBox()->addItem(it->first, it->second);
        if (m_currentAuthAlg == it->second) {
            curAuthAlgOption = it->first;
        }
    }
    m_authAlgChooser->setCurrentText(curAuthAlgOption);

    insertItem(1, m_keyMgmtChooser);
    insertItem(2, m_passwdEdit);
    insertItem(3, m_authAlgChooser);
}

void SecretWirelessSection::initConnection()
{
    connect(m_keyMgmtChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        for (auto it = KeyMgmtStrMap.cbegin(); it != KeyMgmtStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                onKeyMgmtChanged(it->second);
                break;
            }
        }
    });

    connect(m_authAlgChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        for (auto it = AuthAlgStrMap.cbegin(); it != AuthAlgStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                m_currentAuthAlg = it->second;
                break;
            }
        }
    });

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretWirelessSection::saveUserInputPassword);
}

void SecretWirelessSection::onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    if (m_currentKeyMgmt != keyMgmt) {
        m_currentKeyMgmt = keyMgmt;
    }

    switch (m_currentKeyMgmt) {
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone: {
        m_passwdEdit->setVisible(false);
        m_enableWatcher->setSecretEnable(false);
        m_authAlgChooser->setVisible(false);
        showPasswordFlagsChooser(false);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep: {
        m_passwdEdit->setText(m_wsSetting->wepKey0());
        m_passwdEdit->setTitle(tr("Key"));
        m_passwdEdit->setVisible(true);
        m_enableWatcher->setSecretEnable(false);
        m_authAlgChooser->setVisible(true);
        showPasswordFlagsChooser(true);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk: {
        m_passwdEdit->setText(m_wsSetting->psk());
        m_passwdEdit->setTitle(tr("Password"));
        m_passwdEdit->setVisible(true);
        m_enableWatcher->setSecretEnable(false);
        m_authAlgChooser->setVisible(false);
        showPasswordFlagsChooser(true);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap: {
        m_passwdEdit->setVisible(false);
        m_enableWatcher->setSecretEnable(true);
        m_authAlgChooser->setVisible(false);
        break;
    }
    default:
        break;
    }

    if (m_userInputPasswordMap.contains(m_currentKeyMgmt)) {
        m_passwdEdit->setText(m_userInputPasswordMap.value(m_currentKeyMgmt));
    }
}

void SecretWirelessSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentKeyMgmt, m_passwdEdit->text());
}
