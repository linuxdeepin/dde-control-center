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

#ifndef SECRETHOTSPOTSECTION_H
#define SECRETHOTSPOTSECTION_H

#include "abstractsection.h"
#include "widgets/comboboxwidget.h"
#include "widgets/passwdeditwidget.h"

#include <networkmanagerqt/wirelesssecuritysetting.h>

namespace dcc {
namespace network {

class SecretHotspotSection : public AbstractSection
{
    Q_OBJECT

public:
    SecretHotspotSection(NetworkManager::WirelessSecuritySetting::Ptr wsSeting, QFrame *parent = 0);
    virtual ~SecretHotspotSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onKeyMgmtChanged(NetworkManager::WirelessSecuritySetting::KeyMgmt);
    void saveUserInputPassword();

private:
    QMap<QString, NetworkManager::WirelessSecuritySetting::KeyMgmt> KeyMgmtStrMap;

    dcc::widgets::ComboBoxWidget *m_keyMgmtChooser;
    dcc::widgets::PasswdEditWidget *m_passwdEdit;

    NetworkManager::WirelessSecuritySetting::KeyMgmt m_currentKeyMgmt;

    NetworkManager::WirelessSecuritySetting::Ptr m_wsSetting;

    QMap<NetworkManager::WirelessSecuritySetting::KeyMgmt, QString> m_userInputPasswordMap;
};

} /* network */
} /* dcc */

#include "declare_metatype_for_networkmanager.h"
//Q_DECLARE_METATYPE(NetworkManager::WirelessSecuritySetting::KeyMgmt)

#endif /* SECRETHOTSPOTSECTION_H */
