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

#ifndef SECRET8021xSECTION_H
#define SECRET8021xSECTION_H

#include "abstractsection.h"
#include "widgets/comboboxwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/passwdeditwidget.h"

#include <networkmanagerqt/security8021xsetting.h>

namespace dcc {
namespace network {

class Secret8021xEnableWatcher : public QObject
{
    Q_OBJECT

public:
    Secret8021xEnableWatcher (QObject *parent = 0) : QObject(parent) {}
    virtual ~Secret8021xEnableWatcher () {}

    inline bool secretEnabled() {return m_secretEnabled;}

public Q_SLOTS:
    inline void setSecretEnable(const bool enabled) {
        if (m_secretEnabled != enabled) {
            m_secretEnabled = enabled;
            Q_EMIT secretEnableChanged(m_secretEnabled);
        }
    }

Q_SIGNALS:
    void secretEnableChanged(bool enabled);

private:
    bool m_secretEnabled;
};

class Secret8021xSection : public AbstractSection
{
    Q_OBJECT

public:
    virtual ~Secret8021xSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

protected:
    explicit Secret8021xSection(NetworkManager::Security8021xSetting::Ptr sSetting, QFrame *parent = 0);
    // This method must be called by subclass after initialization
    void init(Secret8021xEnableWatcher *watcher, QList<NetworkManager::Security8021xSetting::EapMethod> eapMethodsSupportList);

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void initEapItems(NetworkManager::Security8021xSetting::EapMethod method);
    void initEapMethodTlsItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodFastItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodTtlsItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodPeapItems(QList<dcc::widgets::SettingsItem *> *itemList);

    void onSecretEnableChanged(const bool enable);
    void onEapMethodChanged(NetworkManager::Security8021xSetting::EapMethod method);
    void onPasswordFlagsChanged(NetworkManager::Setting::SecretFlagType type);
    void saveUserInputIdentify();
    void saveUserInputPassword();

    bool commonItemsInpuValid();
    bool tlsItemsInputValid();
    bool fastItemsInputValid();
    bool ttlsItemsInputValid();
    bool peapItemsInputValid();

    void saveCommonItems();
    void saveTlsItems();
    void saveFastItems();
    void saveTtlsItems();
    void savePeapItems();

private:
    QMap<QString, NetworkManager::Security8021xSetting::EapMethod> EapMethodStrMap;
    QMap<QString, NetworkManager::Setting::SecretFlagType> PasswordFlagsStrMap;
    QMap<QString, NetworkManager::Security8021xSetting::FastProvisioning> FastrProvisioningStrMap;
    QMap<QString, NetworkManager::Security8021xSetting::AuthMethod> AuthMethodStrMapFast;
    QMap<QString, NetworkManager::Security8021xSetting::AuthMethod> AuthMethodStrMapTtls;
    QMap<QString, NetworkManager::Security8021xSetting::PeapVersion> PeapVersionStrMap;
    QMap<QString, NetworkManager::Security8021xSetting::AuthMethod> AuthMethodStrMapPeap;

    dcc::widgets::ComboBoxWidget *m_eapMethmodChooser;
    dcc::widgets::ComboBoxWidget *m_passwordFlagsChooser;
    dcc::widgets::LineEditWidget *m_identity;
    dcc::widgets::PasswdEditWidget *m_password;
    Secret8021xEnableWatcher *m_enableWatcher;

    NetworkManager::Security8021xSetting::EapMethod m_currentEapMethod;
    NetworkManager::Setting::SecretFlagType m_currentPasswordType;
    NetworkManager::Security8021xSetting::Ptr m_secretSetting;

    QList<NetworkManager::Security8021xSetting::EapMethod> m_eapMethodsWantedList;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QList<dcc::widgets::SettingsItem *>> m_eapMethodItemsMap;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QString> m_userInputIdentifyMap;
    QMap<NetworkManager::Security8021xSetting::EapMethod, QString> m_userInputPasswordMap;
};

} /* network */
} /* dcc */

#include "declare_metatype_for_networkmanager.h"
//Q_DECLARE_METATYPE(NetworkManager::Security8021xSetting::EapMethod)
//Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType)

#endif /* SECRET8021xSECTION_H */
