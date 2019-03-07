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

#ifndef VPNIPSECSECTION_H
#define VPNIPSECSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/lineeditwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnIpsecSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnIpsecSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnIpsecSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();
    void onIpsecCheckedChanged(const bool enabled);

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::SwitchWidget *m_ipsecEnable;
    dcc::widgets::LineEditWidget *m_groupName;
    dcc::widgets::LineEditWidget *m_gatewayId;
    dcc::widgets::LineEditWidget *m_psk;
    dcc::widgets::LineEditWidget *m_ike;
    dcc::widgets::LineEditWidget *m_esp;
};

} /* network */
} /* dcc */

#endif /* VPNIPSECSECTION_H */
