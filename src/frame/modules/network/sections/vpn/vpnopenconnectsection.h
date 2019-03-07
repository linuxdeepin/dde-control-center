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

#ifndef VPNOPENCONNECTSECTION_H
#define VPNOPENCONNECTSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/filechoosewidget.h"
#include "widgets/switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnOpenConnectSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnOpenConnectSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();

private:
    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::LineEditWidget *m_gateway;
    dcc::widgets::FileChooseWidget *m_caCert;
    dcc::widgets::LineEditWidget *m_proxy;
    dcc::widgets::SwitchWidget *m_enableCSDTrojan;
    dcc::widgets::LineEditWidget *m_csdScript;
    dcc::widgets::FileChooseWidget *m_userCert;
    dcc::widgets::FileChooseWidget *m_userKey;
    dcc::widgets::SwitchWidget *m_useFSID;
};

} /* network */
} /* dcc */

#endif /* VPNOPENCONNECTSECTION_H */
