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

#ifndef SECRETWIRELESSSECTION_H
#define SECRETWIRELESSSECTION_H

#include "secret8021xsection.h"
#include "../parameterscontainer.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

class SecretWirelessSection : public Secret8021xSection
{
    Q_OBJECT

public:
    SecretWirelessSection(WirelessSecuritySetting::Ptr wsSeting, Security8021xSetting::Ptr sSetting, ParametersContainer::Ptr parameter, QFrame *parent = nullptr);
    virtual ~SecretWirelessSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onKeyMgmtChanged(WirelessSecuritySetting::KeyMgmt);
    void saveUserInputPassword();

private:
    QList<QPair<QString, WirelessSecuritySetting::KeyMgmt>> KeyMgmtStrMap;
    QList<QPair<QString, WirelessSecuritySetting::AuthAlg>> AuthAlgStrMap;

    ComboxWidget *m_keyMgmtChooser;
    LineEditWidget *m_passwdEdit;
    Secret8021xEnableWatcher *m_enableWatcher;
    ComboxWidget *m_authAlgChooser;

    WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;
    WirelessSecuritySetting::AuthAlg m_currentAuthAlg;

    WirelessSecuritySetting::Ptr m_wsSetting;
    Security8021xSetting::Ptr m_s8Setting;

    QMap<WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
    ParametersContainer::Ptr m_parameter;
};

#include "declare_metatype_for_networkmanager.h"

#endif /* SECRETWIRELESSSECTION_H */
