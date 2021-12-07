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

#include "secretwirelesssection.h"

#include <QComboBox>

#include <networkmanagerqt/utils.h>

#include <widgets/lineeditwidget.h>
#include <widgets/comboxwidget.h>
#include <widgets/contentwidget.h>

using namespace dcc::widgets;
using namespace NetworkManager;

SecretWirelessSection::SecretWirelessSection(WirelessSecuritySetting::Ptr wsSeting, Security8021xSetting::Ptr sSetting, ParametersContainer::Ptr parameter, QFrame *parent)
    : Secret8021xSection(sSetting, parent)
    , m_keyMgmtChooser(new ComboxWidget(this))
    , m_passwdEdit(new LineEditWidget(true, this))
    , m_enableWatcher(new Secret8021xEnableWatcher(this))
    , m_authAlgChooser(new ComboxWidget(this))
    , m_currentKeyMgmt(WirelessSecuritySetting::KeyMgmt::WpaNone)
    , m_currentAuthAlg(WirelessSecuritySetting::AuthAlg::Shared)
    , m_wsSetting(wsSeting)
    , m_s8Setting(sSetting)
    , m_parameter(parameter)
{
    initStrMaps();

    // init KeyMgmt
    const WirelessSecuritySetting::KeyMgmt &keyMgmt = m_wsSetting->keyMgmt();
    // 当前选择的加密方式
    m_currentKeyMgmt = (keyMgmt == WirelessSecuritySetting::KeyMgmt::Unknown) ?
                       WirelessSecuritySetting::KeyMgmt::WpaNone : keyMgmt;

    // 当前选择的保密模式
    m_currentAuthAlg = (m_wsSetting->authAlg() == WirelessSecuritySetting::AuthAlg::Open) ?
                        m_wsSetting->authAlg() : WirelessSecuritySetting::AuthAlg::Shared;

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Wep) {
        Setting::SecretFlags passwordFlags = m_wsSetting->wepKeyFlags();
        QString strKey = m_wsSetting->wepKey0();
        for (auto it = PasswordFlagsStrMap.cbegin(); it != PasswordFlagsStrMap.cend(); ++it) {
            if (passwordFlags.testFlag(it->second)) {
                m_currentPasswordType = it->second;
                if (m_currentPasswordType == Setting::None && strKey.isEmpty())
                    m_currentPasswordType = Setting::None;

                break;
            }
        }
    } else if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaPsk ||
               m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaSae) {
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

    onKeyMgmtChanged(m_currentKeyMgmt);
}

SecretWirelessSection::~SecretWirelessSection()
{
}

bool SecretWirelessSection::allInputValid()
{
    bool valid = true;

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Wep) {
        if (m_currentPasswordType != Setting::NotSaved) {
            valid = wepKeyIsValid(m_passwdEdit->text(), WirelessSecuritySetting::WepKeyType::Passphrase);
            m_passwdEdit->setIsErr(!valid);
            if (!valid && !m_passwdEdit->text().isEmpty())
                m_passwdEdit->dTextEdit()->showAlertMessage(tr("Invalid password"), this);
        }
    }

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaPsk ||
            m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaSae) {
        if (m_currentPasswordType != Setting::NotSaved) {
            valid = wpaPskIsValid(m_passwdEdit->text());
            m_passwdEdit->setIsErr(!valid);
            if (!valid && !m_passwdEdit->text().isEmpty())
                m_passwdEdit->dTextEdit()->showAlertMessage(tr("Invalid password"), this);
        }
    }

    return valid ? Secret8021xSection::allInputValid() : false;
}

void SecretWirelessSection::saveSettings()
{
    m_wsSetting->setKeyMgmt(m_currentKeyMgmt);

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaNone
            || m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Unknown) {
        return m_wsSetting->setInitialized(false);
    }

    if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::Wep) {
        m_wsSetting->setWepKeyType(WirelessSecuritySetting::WepKeyType::Passphrase);
        m_wsSetting->setWepKeyFlags(m_currentPasswordType);
        if (m_currentPasswordType != Setting::NotSaved)
            m_wsSetting->setWepKey0(m_passwdEdit->text());
        else
            m_wsSetting->setWepKey0(QString());

        m_wsSetting->setPskFlags(Setting::NotRequired);
        m_wsSetting->setAuthAlg(m_currentAuthAlg);
    } else if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaPsk ||
               m_currentKeyMgmt == NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaSae) {
        m_wsSetting->setPskFlags(m_currentPasswordType);
        if (m_currentPasswordType != Setting::NotSaved)
            m_wsSetting->setPsk(m_passwdEdit->text());
        else
            m_wsSetting->setPsk(QString());

        m_wsSetting->setWepKeyType(WirelessSecuritySetting::WepKeyType::NotSpecified);
        m_wsSetting->setAuthAlg(WirelessSecuritySetting::AuthAlg::None);
    } else if (m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaEap) {
        m_wsSetting->setAuthAlg(WirelessSecuritySetting::AuthAlg::None);
    }

    m_wsSetting->setInitialized(true);

    Secret8021xSection::saveSettings();
}

void SecretWirelessSection::initStrMaps()
{
    KeyMgmtStrMap = {
        { tr("None"), WirelessSecuritySetting::KeyMgmt::WpaNone },
        { tr("WEP"), WirelessSecuritySetting::KeyMgmt::Wep },
        { tr("WPA/WPA2 Personal"), WirelessSecuritySetting::KeyMgmt::WpaPsk },
        { tr("WPA/WPA2 Enterprise"), WirelessSecuritySetting::KeyMgmt::WpaEap }
    };

    if (supportWPA3())
        KeyMgmtStrMap << qMakePair(tr("WPA3 Personal"), NetworkManager::WirelessSecuritySetting::KeyMgmt::WpaSae);

    AuthAlgStrMap = {
        { tr("Shared key"), WirelessSecuritySetting::AuthAlg::Shared },
        { tr("Open system"), WirelessSecuritySetting::AuthAlg::Open }
    };
}

void SecretWirelessSection::initUI()
{
    m_keyMgmtChooser->setTitle(tr("Security"));
    QString curMgmtOption = KeyMgmtStrMap.at(0).first;
    for (auto it = KeyMgmtStrMap.cbegin(); it != KeyMgmtStrMap.cend(); ++it) {
        m_keyMgmtChooser->comboBox()->addItem(it->first, it->second);
        if (m_currentKeyMgmt == it->second)
            curMgmtOption = it->first;
    }

    m_keyMgmtChooser->setCurrentText(curMgmtOption);

    m_passwdEdit->setPlaceholderText(tr("Required"));

    m_enableWatcher->setSecretEnable(m_currentKeyMgmt == WirelessSecuritySetting::KeyMgmt::WpaEap);

    QList<Security8021xSetting::EapMethod> eapMethodsWantedList;
    eapMethodsWantedList.append(Security8021xSetting::EapMethod::EapMethodTls);
    eapMethodsWantedList.append(Security8021xSetting::EapMethod::EapMethodLeap);
    eapMethodsWantedList.append(Security8021xSetting::EapMethod::EapMethodFast);
    eapMethodsWantedList.append(Security8021xSetting::EapMethod::EapMethodTtls);
    eapMethodsWantedList.append(Security8021xSetting::EapMethod::EapMethodPeap);

    m_authAlgChooser->setTitle(tr("Authentication"));
    QString curAuthAlgOption = AuthAlgStrMap.at(0).first;
    for (auto it = AuthAlgStrMap.cbegin(); it != AuthAlgStrMap.cend(); ++it) {
        m_authAlgChooser->comboBox()->addItem(it->first, it->second);
        if (m_currentAuthAlg == it->second)
            curAuthAlgOption = it->first;
    }

    m_authAlgChooser->setCurrentText(curAuthAlgOption);

    appendItem(m_keyMgmtChooser);
    appendItem(m_passwordFlagsChooser);
    init(m_enableWatcher, eapMethodsWantedList);
    appendItem(m_passwdEdit);
    appendItem(m_authAlgChooser);

    m_passwdEdit->textEdit()->installEventFilter(this);
}

void SecretWirelessSection::initConnection()
{
    connect(m_keyMgmtChooser, &ComboxWidget::onSelectChanged, this, [ = ] (const QString &dataSelected) {
        for (auto it = KeyMgmtStrMap.cbegin(); it != KeyMgmtStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                onKeyMgmtChanged(it->second);
                break;
            }
        }
    });

    connect(m_authAlgChooser, &ComboxWidget::onSelectChanged, this, [ = ] (const QString &dataSelected) {
        for (auto it = AuthAlgStrMap.cbegin(); it != AuthAlgStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                m_currentAuthAlg = it->second;
                break;
            }
        }
    });

    connect(m_passwdEdit->textEdit(), &QLineEdit::editingFinished, this, &SecretWirelessSection::saveUserInputPassword, Qt::QueuedConnection);
    connect(m_enableWatcher, &Secret8021xEnableWatcher::passwdEnableChanged, this,  [ = ] (const bool enabled) {
        switch (m_currentKeyMgmt) {
        case WirelessSecuritySetting::KeyMgmt::WpaNone:
        case WirelessSecuritySetting::KeyMgmt::WpaEap: {
            m_passwdEdit->setVisible(false);
            break;
        }
        case WirelessSecuritySetting::KeyMgmt::Wep: {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(enabled);
            break;
        }
        case WirelessSecuritySetting::KeyMgmt::WpaPsk:
        case WirelessSecuritySetting::KeyMgmt::WpaSae:{
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(enabled);
            break;
        }
        default:
            break;
        }
    });

    connect(m_keyMgmtChooser, &ComboxWidget::onIndexChanged, this, &SecretWirelessSection::editClicked);
    connect(m_authAlgChooser, &ComboxWidget::onIndexChanged, this, &SecretWirelessSection::editClicked);
}

void SecretWirelessSection::onKeyMgmtChanged(WirelessSecuritySetting::KeyMgmt keyMgmt)
{
    if (m_currentKeyMgmt != keyMgmt)
        m_currentKeyMgmt = keyMgmt;

    switch (m_currentKeyMgmt) {
    case WirelessSecuritySetting::KeyMgmt::WpaNone: {
        m_passwdEdit->setVisible(false);
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(false);
        m_authAlgChooser->setVisible(false);
        break;
    }
    case WirelessSecuritySetting::KeyMgmt::Wep: {
        if (m_currentPasswordType != Setting::NotSaved) {
            m_passwdEdit->setText(m_wsSetting->wepKey0());
            m_passwdEdit->setTitle(tr("Key"));
            m_passwdEdit->setVisible(true);
        } else {
            m_passwdEdit->setVisible(false);
        }
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(true);
        m_authAlgChooser->setVisible(true);
        break;
    }
    case WirelessSecuritySetting::KeyMgmt::WpaPsk: {
        if (m_currentPasswordType != Setting::NotSaved) {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(true);
        } else {
            m_passwdEdit->setVisible(false);
        }
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(true);
        m_authAlgChooser->setVisible(false);
        break;
    }
    case WirelessSecuritySetting::KeyMgmt::WpaEap: {
        m_passwdEdit->setVisible(false);
        m_enableWatcher->setSecretEnable(true);
        m_authAlgChooser->setVisible(false);
        break;
    }
    case WirelessSecuritySetting::KeyMgmt::WpaSae: {
        if (m_currentPasswordType != NetworkManager::Setting::NotSaved) {
            m_passwdEdit->setText(m_wsSetting->psk());
            m_passwdEdit->setTitle(tr("Password"));
            m_passwdEdit->setVisible(true);
        } else {
            m_passwdEdit->setVisible(false);
        }
        m_enableWatcher->setSecretEnable(false);
        m_passwordFlagsChooser->setVisible(true);
        m_authAlgChooser->setVisible(false);
        break;
    }
    default:
        break;
    }

    if (m_userInputPasswordMap.contains(m_currentKeyMgmt))
        m_passwdEdit->setText(m_userInputPasswordMap.value(m_currentKeyMgmt));
}

void SecretWirelessSection::saveUserInputPassword()
{
    m_userInputPasswordMap.insert(m_currentKeyMgmt, m_passwdEdit->text());
}

bool SecretWirelessSection::supportWPA3()
{
    // 若当前连接的是隐藏网络，让显示该项目
    if (m_parameter->getValue(ParametersContainer::ParamType::isHidden).toBool())
        return true;

    AccessPoint::Ptr nmAp = QSharedPointer<AccessPoint>(new AccessPoint(m_parameter->getValue(ParametersContainer::ParamType::AccessPath).toString()));
    AccessPoint::WpaFlags rsn = nmAp->rsnFlags();
    AccessPoint::WpaFlags wpa = nmAp->wpaFlags();
    return (wpa.testFlag(NetworkManager::AccessPoint::WpaFlag::keyMgmtSae) ||
            rsn.testFlag(NetworkManager::AccessPoint::WpaFlag::keyMgmtSae));
}
