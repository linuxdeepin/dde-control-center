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

#ifndef VPNIPSECSECTION_H
#define VPNIPSECSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class SwitchWidget;
    class LineEditWidget;
  }
}

using namespace NetworkManager;

class VpnIpsecSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnIpsecSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnIpsecSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnection();
    void onIpsecCheckedChanged(const bool enabled);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    SwitchWidget *m_ipsecEnable;
    LineEditWidget *m_groupName;
    LineEditWidget *m_gatewayId;
    LineEditWidget *m_psk;
    LineEditWidget *m_ike;
    LineEditWidget *m_esp;
};

#endif /* VPNIPSECSECTION_H */
