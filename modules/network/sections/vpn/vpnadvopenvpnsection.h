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

#ifndef VPNADVOPENVPNSECTION_H
#define VPNADVOPENVPNSECTION_H

#include "../abstractsection.h"
#include "contentwidget.h"
#include "switchwidget.h"
#include "spinboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace widgets {

class VpnAdvOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnAdvOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnAdvOpenVPNSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_portSwitch;
    SwitchWidget *m_renegIntervalSwitch;
    SwitchWidget *m_compLZOSwitch;
    SwitchWidget *m_tcpProtoSwitch;
    SwitchWidget *m_useTapSwitch;
    SwitchWidget *m_tunnelMTUSwitch;
    SwitchWidget *m_udpFragSizeSwitch;
    SwitchWidget *m_restrictMSSSwitch;
    SwitchWidget *m_randomRemoteSwitch;

    SpinBoxWidget *m_port;
    SpinBoxWidget *m_renegInterval;
    SpinBoxWidget *m_tunnelMTU;
    SpinBoxWidget *m_udpFragSize;
};

} /* widgets */
} /* dcc */

#endif /* VPNADVOPENVPNSECTION_H */
