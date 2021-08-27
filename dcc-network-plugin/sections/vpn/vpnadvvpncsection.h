/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
