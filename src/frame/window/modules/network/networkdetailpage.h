/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef NETWORKDETAILPAGE_H
#define NETWORKDETAILPAGE_H

#include "widgets/contentwidget.h"
#include "interface/namespace.h"

#include <networkmanagerqt/ipaddress.h>

using namespace NetworkManager;

namespace DCC_NAMESPACE {
namespace network {

#define DEFAULT_ORDER 98

class NetworkDetailPage : public dcc::ContentWidget
{
    Q_OBJECT
    enum InfoType
    {
        Ip,
        Gateway
    };

public:
    explicit NetworkDetailPage(QWidget *parent = nullptr);

    void updateNetworkInfo();

private Q_SLOTS:
    void onActiveInfoChanged(QStringList &infos);

private:
    QString ipv6Infomation(QJsonObject connectinfos, InfoType type);
private:
    QVBoxLayout *m_groupsLayout;
};
}
}
#endif // NETWORKDETAILPAGE_H
