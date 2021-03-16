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

#include "interface/namespace.h"

#include <DListView>

#include <QPointer>

class QFileDialog;
namespace dde {
namespace network {
class NetworkModel;
}
}

namespace dcc {
class ContentWidget;
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class LoadingNextPageWidget;
}
}

namespace DCC_NAMESPACE {
namespace network {
class ConnectionVpnEditPage;
class VpnPage : public QWidget
{
    Q_OBJECT

public:
    explicit VpnPage(QWidget *parent = nullptr);
    ~VpnPage();
    void jumpPath(const QString &searchPath);

Q_SIGNALS:
    void requestNextPage(dcc::ContentWidget *const w) const;
    void requestVpnEnabled(const bool enabled) const;
    void requestActivateConnection(const QString &devPath, const QString &uuid) const;
    void requestFrameKeepAutoHide(const bool autoHide) const;

public Q_SLOTS:
    void setModel(dde::network::NetworkModel *model);
    void refreshVpnList(const QList<QJsonObject> &vpnList);

private Q_SLOTS:
    void onVpnDetailClicked(const QString &connectionUuid);
    void onVpnSelected(const QModelIndex &idx);
    void onActiveConnsInfoChanged(const QList<QJsonObject> &infos);

    void importVPN();
    void createVPN();
    void changeVpnId();
private:
    dde::network::NetworkModel *m_model;

    dcc::widgets::SwitchWidget *m_vpnSwitch;

    QPointer<ConnectionVpnEditPage> m_editPage;
    QString m_editingConnUuid;

    DTK_WIDGET_NAMESPACE::DListView *m_lvprofiles;
    QStandardItemModel *m_modelprofiles;
    QFileDialog *m_importFile;

    static const int VpnInfoRole = Dtk::UserRole + 1;
};
}
}

#endif // VPNPAGE_H
