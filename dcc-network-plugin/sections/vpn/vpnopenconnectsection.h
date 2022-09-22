// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNOPENCONNECTSECTION_H
#define VPNOPENCONNECTSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class FileChooseWidget;
    class SwitchWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class VpnOpenConnectSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSection(VpnSetting::Ptr vpnSetting, QFrame *parent = Q_NULLPTR);
    virtual ~VpnOpenConnectSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnect();
    bool isIpv4Address(const QString &ip);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_gateway;
    FileChooseWidget *m_caCert;
    LineEditWidget *m_proxy;
    SwitchWidget *m_enableCSDTrojan;
    LineEditWidget *m_csdScript;
    FileChooseWidget *m_userCert;
    FileChooseWidget *m_userKey;
    SwitchWidget *m_useFSID;
};

#endif /* VPNOPENCONNECTSECTION_H */
