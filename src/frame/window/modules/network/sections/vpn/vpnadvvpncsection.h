/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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
#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/comboxwidget.h"
#include "widgets/spinboxwidget.h"
#include "widgets/switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace DCC_NAMESPACE {
namespace network {

class VpnAdvVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnAdvVPNCSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnAdvVPNCSection();

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

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::LineEditWidget *m_domain;
    dcc::widgets::ComboxWidget *m_vendorChooser;
    dcc::widgets::LineEditWidget *m_version;
    dcc::widgets::ComboxWidget *m_encryptionChooser;
    dcc::widgets::ComboxWidget *m_natTravModeChooser;
    dcc::widgets::ComboxWidget *m_ikeDHGroupChooser;
    dcc::widgets::ComboxWidget *m_forwordSecrecyChooser;
    dcc::widgets::SpinBoxWidget *m_localPort;
    dcc::widgets::SwitchWidget *m_disableDPD;

    QString m_currentVendor;
    QString m_currentEncryption;
    QString m_currentNatTravMod;
    QString m_currentIkeDHGroup;
    QString m_currentForwordSecrecy;
};

} /* network */
} /* dcc */

#endif /* VPNADVVPNCSECTION_H */
