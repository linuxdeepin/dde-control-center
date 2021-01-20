/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef DNSSECTION_H
#define DNSSECTION_H

#include "abstractsection.h"

#include <KF5/NetworkManagerQt/networkmanagerqt/connectionsettings.h>

QT_BEGIN_NAMESPACE
class QHostAddress;
QT_END_NAMESPACE

namespace dcc {
namespace widgets {
class SettingsHead;
class ButtonEditWidget;
}
}

namespace DCC_NAMESPACE {
namespace network {
class DNSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit DNSSection(NetworkManager::ConnectionSettings::Ptr connSettings, QFrame *parent = nullptr);
    virtual ~DNSSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;
    bool isIpv4Address(const QString &ip);
    bool isIpv6Address(const QString &ip);

    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initDnsList(const int &num);
    void onDnsDateListChanged();
    QList<QHostAddress> getIPvxDate(); // 获取IPvx dns 数据
    void deleteCurrentDnsEdit(dcc::widgets::ButtonEditWidget *item);
    void addNewDnsEdit(const int &index);
    void onAddBtnStatusChange(const bool &flag);
    void onReduceBtnStatusChange(const bool &flag);

public Q_SLOTS:
    void onBtnShow(const bool &flag);

private:
    QList<QHostAddress> m_recordDns;
    dcc::widgets::SettingsHead *m_headItem;
    QList<dcc::widgets::ButtonEditWidget *> m_itemsList;

    NetworkManager::ConnectionSettings::Ptr m_connSettings;
    NetworkManager::Setting::Ptr m_ipv4Setting;
    NetworkManager::Setting::Ptr m_ipv6Setting;
    QList<QHostAddress> m_ipv4Dns;
    QList<QHostAddress> m_ipv6Dns;
};
}/*network*/
}/*dcc*/

#endif // DNSSECTION_H
