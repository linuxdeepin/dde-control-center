// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNPROXYSECTION_H
#define VPNPROXYSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class SwitchWidget;
    class LineEditWidget;
    class ComboxWidget;
    class SpinBoxWidget;
  }
}
class PasswdLineEditWidget;

using namespace NetworkManager;

class VpnProxySection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnProxySection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnProxySection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onProxyTypeChanged(const QString &type);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, QString>> ProxyTypeStrMap;

    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    ComboxWidget *m_proxyTypeChooser;
    LineEditWidget *m_server;
    SpinBoxWidget *m_port;
    SwitchWidget *m_retry;
    LineEditWidget *m_userName;
    PasswdLineEditWidget *m_password;

    QString m_currentProxyType;
};

#endif /* VPNPROXYSECTION_H */
