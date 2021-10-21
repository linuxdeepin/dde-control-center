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

#ifndef VPNSECTION_H
#define VPNSECTION_H

#include "../abstractsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class ComboxWidget;
  }
}

using namespace NetworkManager;

class VpnSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit VpnSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onPasswordFlagsChanged(Setting::SecretFlagType type);
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    bool isIpv4Address(const QString &ip);

private:
    QList<QPair<QString, Setting::SecretFlagType>> PasswordFlagsStrMap;

    VpnSetting::Ptr m_vpnSetting;
    Setting::SecretFlagType m_currentPasswordType;
    NMStringMap m_dataMap;
    NMStringMap m_secretMap;

    LineEditWidget *m_gateway;
    LineEditWidget *m_userName;
    ComboxWidget *m_passwordFlagsChooser;
    LineEditWidget *m_password;
    LineEditWidget *m_domain;
};

#include "../declare_metatype_for_networkmanager.h"

#endif /* VPNSECTION_H */
