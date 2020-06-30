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
#include "interface/namespace.h"

#include <DButtonBox>
#include <DTextEdit>

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
class ButtonTuple;
class SettingsGroup;
}
}

namespace DCC_NAMESPACE {
namespace network {

class ProxyPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProxyPage(QWidget *parent = nullptr);

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

    QWidget *m_autoWidget;
    QWidget *m_manualWidget;
    dcc::widgets::ButtonTuple *m_buttonTuple;

    dcc::widgets::LineEditWidget *m_httpAddr;
    dcc::widgets::LineEditWidget *m_httpPort;
    dcc::widgets::LineEditWidget *m_httpsAddr;
    dcc::widgets::LineEditWidget *m_httpsPort;
    dcc::widgets::LineEditWidget *m_ftpAddr;
    dcc::widgets::LineEditWidget *m_ftpPort;
    dcc::widgets::LineEditWidget *m_socksAddr;
    dcc::widgets::LineEditWidget *m_socksPort;
    DTextEdit *m_ignoreList;

    dcc::widgets::LineEditWidget *m_autoUrl;

    dcc::widgets::SettingsGroup *m_httpGroup;
    dcc::widgets::SettingsGroup *m_httpsGroup;
    dcc::widgets::SettingsGroup *m_ftpGroup;
    dcc::widgets::SettingsGroup *m_socksGroup;
    DButtonBox *m_proxyTabs;
    QList<DButtonBoxButton *> m_tablist;
};
}
}

#endif // PROXYPAGE_H
