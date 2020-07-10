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
    , m_currentKeyMgmt(NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone)
    , m_wsSetting(wsSeting)
    , m_s8Setting(sSetting)
{
    initStrMaps();

    // init KeyMgmt
    const NetworkManager::WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    m_currentKeyMgmt = (keyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Unknown) ?
                       NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        NetworkManager::Setting::SecretFlags passwordFlags = m_wsSetting->wepKeyFlags();
        QString strKey = m_wsSetting->wepKey0();
        for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
            if (passwordFlags.testFlag(it->second)) {
                m_currentPasswordType = it->second;
                if (m_currentPasswordType == NetworkManager::Setting::None && strKey.isEmpty()) {
                    m_currentPasswordType = NetworkManager::Setting::AgentOwned;
                }
                break;
            }
        }
    } else if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        NetworkManager::Setting::SecretFlags passwordFlags = m_wsSetting->pskFlags();
        QString strKey = m_wsSetting->psk();
        for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
            if (passwordFlags.testFlag(it->second)) {
                m_currentPasswordType = it->second;
                if (m_currentPasswordType == NetworkManager::Setting::None && strKey.isEmpty()) {
                    m_currentPasswordType = NetworkManager::Setting::AgentOwned;
                }
                break;
            }
        }
    }

    initUI();
    initConnection();

    if (!m_wsSetting->psk().isEmpty()) {
        static_cast<DPasswordEdit*>(m_passwdEdit->dTextEdit())->setEchoButtonIsVisible(false);
    }
    connect(m_passwdEdit->dTextEdit(), &DLineEdit::textEdited, this, [ = ](const QString &str) {
        if (str == "")
            static_cast<DPasswordEdit*>(m_passwdEdit->dTextEdit())->setEchoButtonIsVisible(true);
    });

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretWirelessSection::~SecretWirelessSection()
{
}

bool SecretWirelessSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep) {
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            valid = NetworkManager::wepKeyIsValid(m_passwdEdit->text(),
                                                  NetworkManager::WirelessSecuritySetting::WepKeyType::Hex);
            m_passwdEdit->setIsErr(!valid);
            if (!valid && !m_passwdEdit->text().isEmpty()) {
                m_passwdEdit->showAlertMessage(tr("Invalid password"));
            }
        }
    }

    if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            valid = NetworkManager::wpaPskIsValid(m_passwdEdit->text());
            m_passwdEdit->setIsErr(!valid);
            if (!valid && !m_passwdEdit->text().isEmpty()) {
                m_passwdEdit->showAlertMessage(tr("Invalid password"));
            }
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
        m_wsSetting->setWepKeyFlags(m_currentPasswordType);
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            m_wsSetting->setWepKey0(m_passwdEdit->text());
        } else {
            m_wsSetting->setWepKey0(QString());
        }
        m_wsSetting->setPskFlags(NetworkManager::Setting::NotRequired);
    } else if (m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk) {
        m_wsSetting->setPskFlags(m_currentPasswordType);
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            m_wsSetting->setPsk(m_passwdEdit->text());
        } else {
            m_wsSetting->setPsk(QString());
        }
        m_wsSetting->setWepKeyType(NetworkManager::WirelessSecuritySetting::WepKeyType::NotSpecified);
        m_wsSetting->setWepKeyFlags(NetworkManager::Setting::NotRequired);
    }

    m_wsSetting->setInitialized(true);

    Secret8021xSection::saveSettings();
}

void SecretWirelessSection::initStrMaps()
{
    KeyMgmtStrMap = {
        {tr("None"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone},
        {tr("WEP 40/128-bit"), NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep},
        {tr("WPA/WPA2 Personal"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk},
        {tr("WPA/WPA2 Enterprise"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap}
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
    m_passwdEdit->setForbiddenChineseInput();

    m_enableWatcher->setSecretEnable(m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap);

    QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodLeap);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(NetworkManager::Security8021xSetting::EapMethod::EapMethodPeap);

    init(m_enableWatcher, eapMethodsWantedList);

    insertItem(1, m_keyMgmtChooser);
    insertItem(2, m_passwordFlagsChooser);
    insertItem(3, m_passwdEdit);
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

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretWirelessSection::saveUserInputPassword);
    connect(m_enableWatcher, &Secret8021xEnableWatcher::passwdEnableChanged, this,  [ = ](const bool enabled) {
        switch (m_currentKeyMgmt) {
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaNone:
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap: {
            m_passwdEdit->setVisible(false);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep: {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(enabled);
            break;
        }
        case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk: {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(enabled);
            break;
        }
        default:
            break;
        }
    });
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
        m_passwordFlagsChooser->setVisible(false);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::Wep: {
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(true);
        } else {
            m_passwdEdit->setVisible(false);
        }
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(true);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaPsk: {
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(true);
        } else {
            m_passwdEdit->setVisible(false);
        }
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(true);
        break;
    }
    case NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaEap: {
        m_passwdEdit->setVisible(false);
        m_enableWatcher->setSecretEnable(true);
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
