/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
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

using namespace NetworkManager;
using namespace dcc::widgets;

class DNSSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit DNSSection(ConnectionSettings::Ptr connSettings, bool isSupportIPV6 = true, QFrame *parent = nullptr);
    virtual ~DNSSection() Q_DECL_OVERRIDE;

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
    void deleteCurrentDnsEdit(ButtonEditWidget *item);
    void addNewDnsEdit(const int &index);
    void onAddBtnStatusChange(const bool &flag);
    void onReduceBtnStatusChange(const bool &flag);

public Q_SLOTS:
    void onBtnShow(const bool &flag);

private:
    QList<QHostAddress> m_recordDns;
    SettingsHead *m_headItem;
    QList<ButtonEditWidget *> m_itemsList;

    ConnectionSettings::Ptr m_connSettings;
    Setting::Ptr m_ipv4Setting;
    Setting::Ptr m_ipv6Setting;
    QList<QHostAddress> m_ipv4Dns;
    QList<QHostAddress> m_ipv6Dns;
    bool m_supportIPV6;
};

#endif // DNSSECTION_H
