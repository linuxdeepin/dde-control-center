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

#ifndef VPNLISTMODEL_H
#define VPNLISTMODEL_H

#include <QAbstractListModel>
#include <QPixmap>
#include <QJsonObject>

namespace dde {
namespace network {
class NetworkModel;
}
}

class VpnListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum VpnState
    {
        NotActive,
        Activing,
        Actived,
    };

    enum VpnItmeRole
    {
        VpnNameRole = Qt::DisplayRole,
        UnusedRole = Qt::UserRole,
        VpnIconRole,
        VpnStateRole,
        VpnItemHoveredRole,
        VpnUuidRole,
        VpnIsFirstLineRole,
        VpnNextRole,
        VpnDisableRole,
    };

    explicit VpnListModel(dde::network::NetworkModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void refreshActivedIndex();
    void setHoveredIndex(const QModelIndex &index);
    void onActiveConnInfoChanged(const QList<QJsonObject> &infoList);

private:
    bool needRefresh() const;

private:
    VpnState vpnState(const QString &uuid) const;

private:
    const QPixmap m_connectedPixmap;
    const QPixmap m_cancelPixmap;
    QModelIndex m_hoveredIndex;

    QList<QJsonObject> m_activeVpns;

    dde::network::NetworkModel *m_networkModel;
    QTimer *m_refreshTimer;
};

Q_DECLARE_METATYPE(VpnListModel::VpnState)

#endif // VPNLISTMODEL_H
