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
    LineEditWidget *m_password;

    SwitchWidget *m_requestInnerIp;
    SwitchWidget *m_enforceUDP;
    SwitchWidget *m_useIPComp;
    SwitchWidget *m_enableCustomCipher;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;

    QString m_currentAuthType;
};

#endif /* VPNSTRONGSWANSECTION_H */
