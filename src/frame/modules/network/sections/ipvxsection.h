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

#ifndef IPVXSECTION_H
#define IPVXSECTION_H

#include "abstractsection.h"
#include "widgets/comboboxwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/spinboxwidget.h"
#include "widgets/switchwidget.h"

#include <networkmanagerqt/ipv4setting.h>
#include <networkmanagerqt/ipv6setting.h>

namespace dcc {
namespace network {

class IpvxSection : public AbstractSection
{
    Q_OBJECT

public:
    enum Ipvx {Ipv4, Ipv6};

    explicit IpvxSection(NetworkManager::Ipv4Setting::Ptr ipv4Setting, QFrame *parent = 0);
    explicit IpvxSection(NetworkManager::Ipv6Setting::Ptr ipv6Setting, QFrame *parent = 0);
    virtual ~IpvxSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

    bool saveIpv4Settings();
    bool saveIpv6Settings();

    void setIpv4ConfigMethodEnable(NetworkManager::Ipv4Setting::ConfigMethod method, const bool enabled);
    void setIpv6ConfigMethodEnable(NetworkManager::Ipv6Setting::ConfigMethod method, const bool enabled);
    void setNeverDefaultEnable(const bool neverDefault);

private:
    void initStrMaps();
    void initUI();
    void initForIpv4();
    void initForIpv6();
    void initConnection();

    void onIpv4MethodChanged(NetworkManager::Ipv4Setting::ConfigMethod method);
    void onIpv6MethodChanged(NetworkManager::Ipv6Setting::ConfigMethod method);

    bool ipv4InputIsValid();
    bool ipv6InputIsValid();
    bool isIpv4Address(const QString &ip);
    bool isIpv6Address(const QString &ip);
    bool isIpv4SubnetMask(const QString &ip);

    QList<QHostAddress> dnsList();

private:
    QMap<QString, NetworkManager::Ipv4Setting::ConfigMethod> Ipv4ConfigMethodStrMap;
    QMap<QString, NetworkManager::Ipv6Setting::ConfigMethod> Ipv6ConfigMethodStrMap;

    dcc::widgets::ComboBoxWidget *m_methodChooser;
    dcc::widgets::LineEditWidget *m_ipAddress;
    dcc::widgets::LineEditWidget *m_netmaskIpv4;
    dcc::widgets::SpinBoxWidget *m_prefixIpv6;
    dcc::widgets::LineEditWidget *m_gateway;
    dcc::widgets::LineEditWidget *m_dnsPrimary;
    dcc::widgets::LineEditWidget *m_dnsSecond;
    dcc::widgets::SwitchWidget *m_neverDefault;

    QList<dcc::widgets::SettingsItem *> m_itemsList;

    Ipvx m_currentIpvx;
    NetworkManager::Setting::Ptr m_ipvxSetting;
};

} /* network */
} /* dcc */

Q_DECLARE_METATYPE(NetworkManager::Ipv4Setting::ConfigMethod)
Q_DECLARE_METATYPE(NetworkManager::Ipv6Setting::ConfigMethod)

#endif /* IPVXSECTION_H */
