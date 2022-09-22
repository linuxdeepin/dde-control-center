// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SECRETWIRELESSSECTION_H
#define SECRETWIRELESSSECTION_H

#include "secret8021xsection.h"
#include "../parameterscontainer.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

class PasswdLineEditWidget;

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
    PasswdLineEditWidget *m_passwdEdit;
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
