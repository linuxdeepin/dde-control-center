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

#ifndef VPNSTRONGSWANSECTION_H
#define VPNSTRONGSWANSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "lineeditwidget.h"
#include "filechoosewidget.h"
#include "comboboxwidget.h"
#include "passwdeditwidget.h"
#include "switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnStrongSwanSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnStrongSwanSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnStrongSwanSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onAuthTypeChanged(const QString type);
    void onCustomCipherEnableChanged(const bool enabled);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    FileChooseWidget *m_caCert;
    ComboBoxWidget *m_authTypeChooser;

    FileChooseWidget *m_userCert;
    FileChooseWidget *m_userKey;
    LineEditWidget *m_userName;
    PasswdEditWidget *m_password;

    SwitchWidget *m_requestInnerIp;
    SwitchWidget *m_enforceUDP;
    SwitchWidget *m_useIPComp;
    SwitchWidget *m_enableCustomCipher;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;

    QString m_currentAuthType;
};

} /* widgets */
} /* dcc */

#endif /* VPNSTRONGSWANSECTION_H */
