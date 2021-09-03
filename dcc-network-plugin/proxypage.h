/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

namespace dde {
  namespace network {
    class SysProxyConfig;
    enum class ProxyMethod;
  }
}

namespace dcc {
  namespace widgets {
    class LineEditWidget;
    class PlainTextItem;
    class ButtonTuple;
    class SwitchWidget;
    class ComboxWidget;
  }
}

DWIDGET_BEGIN_NAMESPACE
class DButtonBox;
class DTextEdit;
DWIDGET_END_NAMESPACE

using namespace dde::network;
using namespace dcc::widgets;
DWIDGET_USE_NAMESPACE

class ProxyPage : public QWidget
{
    Q_OBJECT

public:
    explicit ProxyPage(QWidget *parent = nullptr);
    ~ProxyPage() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestFrameKeepAutoHide(const bool autoHide) const;

private Q_SLOTS:
    void onProxyMethodChanged(const ProxyMethod &method);
    void onIgnoreHostsChanged(const QString &hosts);
    void onProxyChanged(const SysProxyConfig &config);
    void applySettings();

private:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

    void clearLineEditWidgetFocus();

private:
    QWidget *m_autoWidget;
    QWidget *m_manualWidget;
    ButtonTuple *m_buttonTuple;

    LineEditWidget *m_httpAddr;
    LineEditWidget *m_httpPort;
    LineEditWidget *m_httpsAddr;
    LineEditWidget *m_httpsPort;
    LineEditWidget *m_ftpAddr;
    LineEditWidget *m_ftpPort;
    LineEditWidget *m_socksAddr;
    LineEditWidget *m_socksPort;
    DTextEdit *m_ignoreList;

    LineEditWidget *m_autoUrl;

    SwitchWidget* m_proxySwitch;
    ComboxWidget* m_proxyTypeBox;
};

#endif // PROXYPAGE_H
