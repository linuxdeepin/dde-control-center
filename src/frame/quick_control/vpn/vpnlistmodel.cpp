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

#include "vpnlistmodel.h"

#include "widgets/basiclistdelegate.h"

#include <QDebug>
#include <QSize>
#include <QJsonObject>
#include <networkmodel.h>

using dde::network::NetworkModel;

VpnListModel::VpnListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),
      m_connectedPixmap(loadPixmap(":/frame/themes/dark/icons/select.svg")),
      m_cancelPixmap(loadPixmap(":/frame/themes/dark/icons/disconnect.svg")),
      m_networkModel(model),
      m_refreshTimer(new QTimer(this))
{
    m_refreshTimer->setSingleShot(false);
    m_refreshTimer->setInterval(1000 / 60);

    connect(m_refreshTimer, &QTimer::timeout, this, &VpnListModel::refreshActivedIndex);
    connect(m_networkModel, &NetworkModel::activeConnInfoChanged, this, &VpnListModel::onActiveConnInfoChanged);
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, [this] { emit layoutChanged(); });

    onActiveConnInfoChanged(m_networkModel->activeConnInfos());
}

int VpnListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_networkModel->vpnEnabled())
        return m_networkModel->vpns().size();
    else
        return 1;
}

QVariant VpnListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case VpnNameRole:
    {
        if (!m_networkModel->vpnEnabled())
            return tr("Click icon to enable VPN");

        return m_networkModel->vpns()[index.row()].value("Id").toString();
    }
    case VpnUuidRole:           return m_networkModel->vpns()[index.row()].value("Uuid").toString();
    case VpnIconRole:           return m_hoveredIndex == index ? m_cancelPixmap : m_connectedPixmap;
    case VpnStateRole:          return QVariant::fromValue(vpnState(index.data(VpnUuidRole).toString()));
    case VpnItemHoveredRole:    return m_hoveredIndex == index;
    case Qt::SizeHintRole:      return QSize(0, 36);
    case VpnIsFirstLineRole:    return !index.row();
    case VpnNextRole:           return index.row() == m_hoveredIndex.row() + 1;
    case VpnDisableRole:        return  m_networkModel->vpnEnabled() ? false : true;
    default:;
    }

    return QVariant();
}

void VpnListModel::refreshActivedIndex()
{
    const auto vpns = m_networkModel->vpns();

    auto vpnIndex = [&](const QString &uuid)
    {
        for (int i(0); i != vpns.size(); ++i)
            if (vpns[i].value("Uuid").toString() == uuid)
                return index(i, 0);
        return QModelIndex();
    };

    for (const auto &info : m_activeVpns)
    {
        if (info.value("State").toInt() == 2)
            continue;

        const QString &connUuid = info.value("Uuid").toString();
        const auto idx = vpnIndex(connUuid);

        emit dataChanged(idx, idx);
    }

    emit layoutChanged();
}

void VpnListModel::setHoveredIndex(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_hoveredIndex;

    m_hoveredIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_hoveredIndex, m_hoveredIndex);
}

void VpnListModel::onActiveConnInfoChanged(const QList<QJsonObject> &infoList)
{
    m_activeVpns.clear();
    for (const auto &info : infoList)
    {
        const QString &type = info.value("ConnectionType").toString();
        if (!type.startsWith("vpn"))
            continue;

        const QString &uuid = info.value("ConnectionUuid").toString();
        m_activeVpns << m_networkModel->activeConnObjectByUuid(uuid);
    }

    if (needRefresh())
        m_refreshTimer->start();
    else
        m_refreshTimer->stop();

    emit layoutChanged();
}

bool VpnListModel::needRefresh() const
{
    for (const auto &info : m_activeVpns)
    {
        if (info.value("State").toInt() != 2)
            return true;
    }

    return false;
}

VpnListModel::VpnState VpnListModel::vpnState(const QString &uuid) const
{
    if (!m_networkModel->activeConnections().contains(uuid))
        return VpnState::NotActive;

    for (const auto &info : m_activeVpns)
    {
        const QString &connUuid = info.value("Uuid").toString();
        if (connUuid != uuid)
            continue;

        const int state = info.value("State").toInt();
        return state == 2 ? VpnListModel::Actived : VpnListModel::Activing;
    }

    return VpnState::NotActive;
}
