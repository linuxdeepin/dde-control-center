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

#ifndef SECRET8021xSECTION_H
#define SECRET8021xSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/security8021xsetting.h>

namespace dcc {
  namespace widgets {
    class ComboxWidget;
    class LineEditWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class Secret8021xEnableWatcher : public QObject
{
    Q_OBJECT

public:
    Secret8021xEnableWatcher(QObject *parent = nullptr) : QObject(parent), m_secretEnabled(false) {}
    virtual ~Secret8021xEnableWatcher() {}

    inline bool secretEnabled() { return m_secretEnabled; }

public Q_SLOTS:
    inline void setSecretEnable(const bool enabled) {
        if (m_secretEnabled != enabled) {
            m_secretEnabled = enabled;
            Q_EMIT secretEnableChanged(m_secretEnabled);
        }
    }

Q_SIGNALS:
    void secretEnableChanged(bool enabled);
    void passwdEnableChanged(bool enabled);

private:
    bool m_secretEnabled;
};

class Secret8021xSection : public AbstractSection
{
    Q_OBJECT

public:
    virtual ~Secret8021xSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

protected:
    explicit Secret8021xSection(Security8021xSetting::Ptr sSetting, QFrame *parent = nullptr);
    // This method must be called by subclass after initialization
    void init(Secret8021xEnableWatcher *watcher, const QList<Security8021xSetting::EapMethod> &eapMethodsSupportList);
    ComboxWidget *m_passwordFlagsChooser;
    Setting::SecretFlagType m_currentPasswordType;
    QList<QPair<QString, Setting::SecretFlagType>> PasswordFlagsStrMap;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void initEapItems(Security8021xSetting::EapMethod method);
    void initEapMethodTlsItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodFastItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodTtlsItems(QList<dcc::widgets::SettingsItem *> *itemList);
    void initEapMethodPeapItems(QList<dcc::widgets::SettingsItem *> *itemList);

    void onSecretEnableChanged(const bool enable);
    void onEapMethodChanged(Security8021xSetting::EapMethod method);
    void onPasswordFlagsChanged(Setting::SecretFlagType type);
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

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, Security8021xSetting::EapMethod>> EapMethodStrMap;
    QList<QPair<QString, Security8021xSetting::FastProvisioning>> FastrProvisioningStrMap;
    QList<QPair<QString, Security8021xSetting::AuthMethod>> AuthMethodStrMapFast;
    QList<QPair<QString, Security8021xSetting::AuthMethod>> AuthMethodStrMapTtls;
    QList<QPair<QString, Security8021xSetting::PeapVersion>> PeapVersionStrMap;
    QList<QPair<QString, Security8021xSetting::AuthMethod>> AuthMethodStrMapPeap;

    ComboxWidget *m_eapMethmodChooser;
    LineEditWidget *m_identity;
    LineEditWidget *m_password;
    Secret8021xEnableWatcher *m_enableWatcher;

    Security8021xSetting::EapMethod m_currentEapMethod;
    Security8021xSetting::Ptr m_secretSetting;

    QList<Security8021xSetting::EapMethod> m_eapMethodsWantedList;
    QMap<Security8021xSetting::EapMethod, QList<SettingsItem *>> m_eapMethodItemsMap;
    QMap<Security8021xSetting::EapMethod, QString> m_userInputIdentifyMap;
    QMap<Security8021xSetting::EapMethod, QString> m_userInputPasswordMap;
};

#include "declare_metatype_for_networkmanager.h"

#endif /* SECRET8021xSECTION_H */
