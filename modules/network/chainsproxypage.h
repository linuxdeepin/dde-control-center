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

#ifndef CHAINSPROXYPAGE_H
#define CHAINSPROXYPAGE_H

#include "networkmodel.h"
#include "contentwidget.h"

#include <QObject>

namespace dcc {

namespace widgets {
class NextPageWidget;
class LineEditWidget;
}

namespace network {

class ChainsProxyPage : public ContentWidget
{
    Q_OBJECT
public:
    explicit ChainsProxyPage(QWidget *parent = nullptr);
    void setModel(NetworkModel *model);

signals:
    void requestSet(const ProxyConfig &config) const;
    void requestShowTypePage() const;

private slots:
    void onCheckValue();
    bool isIPV4(const QString &ipv4);

private:
    NetworkModel *m_model;
    dcc::widgets::NextPageWidget *m_proxyType;
    dcc::widgets::LineEditWidget *m_addr;
    dcc::widgets::LineEditWidget *m_port;
    dcc::widgets::LineEditWidget *m_username;
    dcc::widgets::LineEditWidget *m_password;

};
}
}

#endif // CHAINSPROXYPAGE_H
