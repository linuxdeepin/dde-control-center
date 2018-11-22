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
 *             listenerri <listenerri@gmail.com>
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

#ifndef VPNPAGE_H
#define VPNPAGE_H

#include "widgets/contentwidget.h"
#include "connectionvpneditpage.h"

#include <QList>
#include <QJsonObject>
#include <QMap>
#include <QPointer>

namespace dde {
namespace network {
class NetworkModel;
}
}

namespace dcc {

namespace widgets {

class SettingsGroup;
class SwitchWidget;
class NextPageWidget;
class LoadingNextPageWidget;

}

namespace network {

class VpnPage : public ContentWidget
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = 0);
    ~VpnPage();

Q_SIGNALS:
    void requestNextPage(ContentWidget * const w) const;
    void requestVpnEnabled(const bool enabled) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void setModel(dde::network::NetworkModel *model);
    void refreshVpnList(const QList<QJsonObject> &vpnList);

private Q_SLOTS:
    void onVpnDetailClicked();
    void onVpnSelected();
    void onVpnEditFinished();
    void onActiveConnsInfoChanged(const QList<QJsonObject> &infos);

    void importVPN();
    void createVPNSession();
    void createVPN(ConnectionVpnEditPage::VpnType vpnType);

private:
    dde::network::NetworkModel *m_model;

    widgets::SwitchWidget *m_vpnSwitch;
    widgets::SettingsGroup *m_vpnGroup;

    QPointer<ConnectionVpnEditPage> m_editPage;
    QString m_editingConnUuid;
    ContentWidget *m_vpnTypePage;

    QMap<widgets::LoadingNextPageWidget *, QJsonObject> m_vpns;
};

}

}

#endif // VPNPAGE_H
