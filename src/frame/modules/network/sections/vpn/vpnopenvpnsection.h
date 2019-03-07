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

#ifndef VPNOPENVPNSECTION_H
#define VPNOPENVPNSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/comboboxwidget.h"
#include "widgets/filechoosewidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnOpenVPNSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    inline QString authType() const {return m_currentAuthType;}

Q_SIGNALS:
    void authTypeChanged(const QString &type) const;

private:
    void initStrMaps();
    void initUI();
    void initItems(const QString &type);
    void initTLSItems();
    void initPasswordItems();
    void initStaticKeyItems();
    void initConnection();
    void onAuthTypeChanged(const QString &type);
    void setItemsVisible(const QString &itemsType, const bool visible);
    bool tlsItemsInputValid();
    bool passwordItemsInputValid();
    bool staticKeyItemsInputValid();
    void saveTlsItems();
    void savePasswordItems();
    void saveStaticKeyItems();

private:
    QMap<QString, QString> AuthTypeStrMap;
    QMap<QString, NetworkManager::Setting::SecretFlagType> PasswordFlagsStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    dcc::widgets::LineEditWidget *m_gateway;
    dcc::widgets::ComboBoxWidget *m_authTypeChooser;
    dcc::widgets::FileChooseWidget *m_caFile;

    QMap<QString, QList<dcc::widgets::SettingsItem *>> m_settingItemsMap;
    QString m_currentAuthType;
    QString m_currentKeyDirection;
    NetworkManager::Setting::SecretFlagType m_currentPasswordType;
    NetworkManager::Setting::SecretFlagType m_currentCertPasswordType;
};

} /* network */
} /* dcc */

#include "../declare_metatype_for_networkmanager.h"
//Q_DECLARE_METATYPE(NetworkManager::Setting::SecretFlagType)

#endif /* VPNOPENVPNSECTION_H */
