// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNIPSECSECTION_H
#define VPNIPSECSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class SwitchWidget;
    class LineEditWidget;
  }
}

using namespace NetworkManager;

class VpnIpsecSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnIpsecSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnIpsecSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();
    void onIpsecCheckedChanged(const bool enabled);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_ipsecEnable;
    LineEditWidget *m_groupName;
    LineEditWidget *m_gatewayId;
    LineEditWidget *m_psk;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;
};

#endif /* VPNIPSECSECTION_H */
