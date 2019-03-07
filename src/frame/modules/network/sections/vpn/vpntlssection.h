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

#ifndef VPNTLSSECTION_H
#define VPNTLSSECTION_H

#include "../abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/comboboxwidget.h"
#include "widgets/filechoosewidget.h"
#include "widgets/switchwidget.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
namespace network {

class VpnTLSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnTLSSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent = 0);
    virtual ~VpnTLSSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();

private:
    QMap<QString, QString> RemoteCertTypeStrMap;

    NetworkManager::VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    dcc::widgets::LineEditWidget *m_remote;
    dcc::widgets::ComboBoxWidget *m_remoteCertTypeChooser;
    dcc::widgets::FileChooseWidget *m_caCert;
    dcc::widgets::SwitchWidget *m_customKeyDirection;
    dcc::widgets::ComboBoxWidget *m_keyDirectionChooser;

    QString m_currentRemoteCertType;
    QString m_currentKeyDirection;
};

} /* network */
} /* dcc */

#endif /* VPNTLSSECTION_H */
