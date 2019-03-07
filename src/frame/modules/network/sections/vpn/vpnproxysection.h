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

#ifndef VPNPROXYSECTION_H
#define VPNPROXYSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/comboboxwidget.h"
#include "widgets/passwdeditwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/spinboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnProxySection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnProxySection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnProxySection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onProxyTypeChanged(const QString &type);

private:
    QMap<QString, QString> ProxyTypeStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    dcc::widgets::ComboBoxWidget *m_proxyTypeChooser;
    dcc::widgets::LineEditWidget *m_server;
    dcc::widgets::SpinBoxWidget *m_port;
    dcc::widgets::SwitchWidget *m_retry;
    dcc::widgets::LineEditWidget *m_userName;
    dcc::widgets::PasswdEditWidget *m_password;

    QString m_currentProxyType;
};

} /* network */
} /* dcc */

#endif /* VPNPROXYSECTION_H */
