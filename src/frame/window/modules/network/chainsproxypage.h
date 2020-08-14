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

#include "widgets/contentwidget.h"
#include "interface/namespace.h"
#include <QComboBox>
#include <QObject>

#include "widgets/buttontuple.h"

namespace dde {
namespace network {
class NetworkModel;
struct ProxyConfig;
}
}

namespace dcc {

namespace widgets {
class ComboxWidget;
class LineEditWidget;
}
}

namespace DCC_NAMESPACE {
namespace network {

class ChainsProxyPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit ChainsProxyPage(QWidget *parent = nullptr);
    void setModel(dde::network::NetworkModel *model);

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
Q_SIGNALS:
    void requestSet(const dde::network::ProxyConfig &config) const;
    void requestShowTypePage() const;

private Q_SLOTS:
    void onCheckValue();
    void onRestoreValue();
    bool isIPV4(const QString &ipv4);

private:
    dde::network::NetworkModel *m_model;
    dcc::widgets::ComboxWidget *m_proxyType;
    dcc::widgets::LineEditWidget *m_addr;
    dcc::widgets::LineEditWidget *m_port;
    dcc::widgets::LineEditWidget *m_username;
    dcc::widgets::LineEditWidget *m_password;
    QComboBox *m_comboBox;
    dcc::widgets::ButtonTuple *m_btns;
};
}
}

#endif // CHAINSPROXYPAGE_H
