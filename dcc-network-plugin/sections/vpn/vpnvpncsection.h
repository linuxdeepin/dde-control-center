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

#ifndef VPNVPNCSECTION_H
#define VPNVPNCSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
    class SwitchWidget;
    class FileChooseWidget;
  }
}

using namespace NetworkManager;

class VpnVPNCSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnVPNCSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnVPNCSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(Setting::SecretFlagType type);
    void onGroupPasswordFlagsChanged(Setting::SecretFlagType type);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QList<QPair<QString, Setting::SecretFlagType>> PasswordFlagsStrMap;

    VpnSetting::Ptr m_vpnSetting;
    Setting::SecretFlagType m_currentPasswordType;
    Setting::SecretFlagType m_currentGroupPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboxWidget *m_passwordFlagsChooser;
    LineEditWidget *m_password;

    LineEditWidget *m_groupName;
    ComboxWidget *m_groupPasswordFlagsChooser;
    LineEditWidget *m_groupPassword;
    SwitchWidget *m_userHybrid;
    FileChooseWidget *m_caFile;
};

#include "../declare_metatype_for_networkmanager.h"

#endif /* VPNVPNCSECTION_H */
