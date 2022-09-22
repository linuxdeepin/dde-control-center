// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSECOPENVPNSECTION_H
#define VPNSECOPENVPNSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class ComboxWidget;
  }
}

using namespace NetworkManager;

class VpnSecOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSecOpenVPNSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnSecOpenVPNSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();

private:
    QList<QPair<QString, QString>> CipherStrMap;
    QList<QPair<QString, QString>> HMACStrMap;

    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    ComboxWidget *m_cipherChooser;
    ComboxWidget *m_hmacChooser;

    QString m_currentCipher;
    QString m_currentHMAC;
};

#endif /* VPNSECOPENVPNSECTION_H */
