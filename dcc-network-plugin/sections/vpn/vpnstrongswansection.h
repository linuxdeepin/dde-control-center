// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSTRONGSWANSECTION_H
#define VPNSTRONGSWANSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class FileChooseWidget;
    class ComboxWidget;
    class SwitchWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class VpnStrongSwanSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnStrongSwanSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnStrongSwanSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onAuthTypeChanged(const QString &type);
    void onCustomCipherEnableChanged(const bool enabled);
    bool isIpv4Address(const QString &ip);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, QString>> AuthTypeStrMap;

    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    FileChooseWidget *m_caCert;
    ComboxWidget *m_authTypeChooser;

    FileChooseWidget *m_userCert;
    FileChooseWidget *m_userKey;
    LineEditWidget *m_userName;
    PasswdLineEditWidget *m_password;

    SwitchWidget *m_requestInnerIp;
    SwitchWidget *m_enforceUDP;
    SwitchWidget *m_useIPComp;
    SwitchWidget *m_enableCustomCipher;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;

    QString m_currentAuthType;
};

#endif /* VPNSTRONGSWANSECTION_H */
