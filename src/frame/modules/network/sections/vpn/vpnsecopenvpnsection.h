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

#ifndef VPNSECOPENVPNSECTION_H
#define VPNSECOPENVPNSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/comboboxwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnSecOpenVPNSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSecOpenVPNSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnSecOpenVPNSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();

private:
    QMap<QString, QString> CipherStrMap;
    QMap<QString, QString> HMACStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::ComboBoxWidget *m_cipherChooser;
    dcc::widgets::ComboBoxWidget *m_hmacChooser;

    QString m_currentCipher;
    QString m_currentHMAC;
};

} /* network */
} /* dcc */

#endif /* VPNSECOPENVPNSECTION_H */
