// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNADVOPENVPNSECTION_H
#define VPNADVOPENVPNSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class SwitchWidget;
    class SpinBoxWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class VpnAdvOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnAdvOpenVPNSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnAdvOpenVPNSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();
    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_portSwitch;
    SwitchWidget *m_renegIntervalSwitch;
    SwitchWidget *m_compLZOSwitch;
    SwitchWidget *m_tcpProtoSwitch;
    SwitchWidget *m_useTapSwitch;
    SwitchWidget *m_tunnelMTUSwitch;
    SwitchWidget *m_udpFragSizeSwitch;
    SwitchWidget *m_restrictMSSSwitch;
    SwitchWidget *m_randomRemoteSwitch;

    SpinBoxWidget *m_port;
    SpinBoxWidget *m_renegInterval;
    SpinBoxWidget *m_tunnelMTU;
    SpinBoxWidget *m_udpFragSize;
};

#endif /* VPNADVOPENVPNSECTION_H */
