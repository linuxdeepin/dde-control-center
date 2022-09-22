// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNVPNCSECTION_H
#define VPNVPNCSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
    class SwitchWidget;
    class FileChooseWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class VpnVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnVPNCSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnVPNCSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(Setting::SecretFlagType type);
    void onGroupPasswordFlagsChanged(Setting::SecretFlagType type);
    bool isIpv4Address(const QString &ip);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, Setting::SecretFlagType>> PasswordFlagsStrMap;

    VpnSetting::Ptr m_vpnSetting;
    Setting::SecretFlagType m_currentPasswordType;
    Setting::SecretFlagType m_currentGroupPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboxWidget *m_passwordFlagsChooser;
    PasswdLineEditWidget *m_password;

    LineEditWidget *m_groupName;
    ComboxWidget *m_groupPasswordFlagsChooser;
    PasswdLineEditWidget *m_groupPassword;
    SwitchWidget *m_userHybrid;
    FileChooseWidget *m_caFile;
};

#include "../declare_metatype_for_networkmanager.h"

#endif /* VPNVPNCSECTION_H */
