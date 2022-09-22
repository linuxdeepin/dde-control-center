// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSECTION_H
#define VPNSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class VpnSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(Setting::SecretFlagType type);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    bool isIpv4Address(const QString &ip);

private:
    QList<QPair<QString, Setting::SecretFlagType>> PasswordFlagsStrMap;

    VpnSetting::Ptr m_vpnSetting;
    Setting::SecretFlagType m_currentPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboxWidget *m_passwordFlagsChooser;
    PasswdLineEditWidget *m_password;
    LineEditWidget *m_domain;
};

#include "../declare_metatype_for_networkmanager.h"

#endif /* VPNSECTION_H */
