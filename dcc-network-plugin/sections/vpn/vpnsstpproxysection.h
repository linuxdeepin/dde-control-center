// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSSTPPROXYSECTION_H
#define VPNSSTPPROXYSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class SpinBoxWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class VpnSstpProxySection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSstpProxySection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnSstpProxySection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_server;
    SpinBoxWidget *m_port;
    LineEditWidget *m_userName;
    PasswdLineEditWidget *m_password;
};

#endif /* VPNSSTPPROXYSECTION_H */
