// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNADVVPNCSECTION_H
#define VPNADVVPNCSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
    class SpinBoxWidget;
    class SwitchWidget;
  }
}

using namespace NetworkManager;

class VpnAdvVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnAdvVPNCSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnAdvVPNCSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, QString>> VendorStrMap;
    QList<QPair<QString, QString>> EncryptionStrMap;
    QList<QPair<QString, QString>> NATTravModeStrMap;
    QList<QPair<QString, QString>> IKEDHGroupStrMap;
    QList<QPair<QString, QString>> ForwardSecrecyStrMap;

    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_domain;
    ComboxWidget *m_vendorChooser;
    LineEditWidget *m_version;
    ComboxWidget *m_encryptionChooser;
    ComboxWidget *m_natTravModeChooser;
    ComboxWidget *m_ikeDHGroupChooser;
    ComboxWidget *m_forwordSecrecyChooser;
    SpinBoxWidget *m_localPort;
    SwitchWidget *m_disableDPD;

    QString m_currentVendor;
    QString m_currentEncryption;
    QString m_currentNatTravMod;
    QString m_currentIkeDHGroup;
    QString m_currentForwordSecrecy;
};

#endif /* VPNADVVPNCSECTION_H */
