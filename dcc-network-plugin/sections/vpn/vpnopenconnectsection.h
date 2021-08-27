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

#ifndef VPNOPENCONNECTSECTION_H
#define VPNOPENCONNECTSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class FileChooseWidget;
    class SwitchWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class VpnOpenConnectSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSection(VpnSetting::Ptr vpnSetting, QFrame *parent = Q_NULLPTR);
    virtual ~VpnOpenConnectSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();
    void initConnect();
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    LineEditWidget *m_gateway;
    FileChooseWidget *m_caCert;
    LineEditWidget *m_proxy;
    SwitchWidget *m_enableCSDTrojan;
    LineEditWidget *m_csdScript;
    FileChooseWidget *m_userCert;
    FileChooseWidget *m_userKey;
    SwitchWidget *m_useFSID;
};

#endif /* VPNOPENCONNECTSECTION_H */
