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

#ifndef PROXYPAGE_H
#define PROXYPAGE_H

#include "widgets/contentwidget.h"
#include "widgets/buttontuple.h"

#include <dsegmentedcontrol.h>

namespace dde {
namespace network {
class NetworkModel;
struct ProxyConfig;
}
}

namespace dcc {

namespace widgets {

class LineEditWidget;
class PlainTextItem;

}

namespace network {

class ProxyPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit ProxyPage(QWidget *parent = 0);

    void setModel(dde::network::NetworkModel *model);

Q_SIGNALS:
    void requestQueryProxyData() const;
    void requestSetProxy(const QString &type, const QString &addr, const QString &port) const;
    void requestSetProxyMethod(const QString &pm) const;
    void requestSetAutoProxy(const QString &proxy) const;
    void requestSetIgnoreHosts(const QString &list) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

private Q_SLOTS:
    void onProxyMethodChanged(const QString &proxyMethod);
    void onIgnoreHostsChanged(const QString &hosts);
    void onProxyChanged(const QString &type, const dde::network::ProxyConfig &config);
    void onProxyToggled(const int index);
    void applySettings() const;

private:
    void applyProxy(const QString &type);

private:
    dde::network::NetworkModel *m_model;

    QWidget *m_manualWidget;
    QWidget *m_autoWidget;
    widgets::ButtonTuple *m_buttonTuple;

    widgets::LineEditWidget *m_httpAddr;
    widgets::LineEditWidget *m_httpPort;
    widgets::LineEditWidget *m_httpsAddr;
    widgets::LineEditWidget *m_httpsPort;
    widgets::LineEditWidget *m_ftpAddr;
    widgets::LineEditWidget *m_ftpPort;
    widgets::LineEditWidget *m_socksAddr;
    widgets::LineEditWidget *m_socksPort;
    widgets::PlainTextItem *m_ignoreList;

    widgets::LineEditWidget *m_autoUrl;

    Dtk::Widget::DSegmentedControl *m_proxyType;
};

}

}

#endif // PROXYPAGE_H
