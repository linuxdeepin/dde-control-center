/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

namespace dcc {

namespace network {

class NetworkModel;

}

}

class VpnListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum VpnItmeRole
    {
        VpnNameRole = Qt::DisplayRole,
        UnusedRole = Qt::UserRole,
        VpnShowIconRole,
        VpnIconRole,
        VpnItemHoveredRole,
        VpnUuidRole,
        VpnIsFirstLineRole,
        VpnNextRole,
        VpnDisableRole,
    };

    explicit VpnListModel(dcc::network::NetworkModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void setHoveredIndex(const QModelIndex &index);
    void onActivedListChanged(const QSet<QString> &activeConnections);

private:
    const QPixmap m_connectedPixmap;
    const QPixmap m_cancelPixmap;
    QModelIndex m_hoveredIndex;

    QList<QString> m_activedVpns;

    dcc::network::NetworkModel *m_networkModel;
};

#endif // VPNLISTMODEL_H
