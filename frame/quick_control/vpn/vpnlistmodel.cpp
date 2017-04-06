#include "vpnlistmodel.h"

#include "network/networkmodel.h"

#include <QDebug>
#include <QSize>
#include <QJsonObject>

using dcc::network::NetworkModel;

VpnListModel::VpnListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),
      m_connectedPixmap(QPixmap(":/frame/themes/dark/icons/select.png")),
      m_cancelPixmap(QPixmap(":/frame/themes/dark/icons/disconnect.png")),
      m_networkModel(model)
{
    connect(m_networkModel, &NetworkModel::activeConnectionsChanged, this, &VpnListModel::onActivedListChanged);
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, [this] { emit layoutChanged(); });

    onActivedListChanged(m_networkModel->activeConnections());
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
    case VpnNameRole: {
        if (!m_networkModel->vpnEnabled())
            return tr("Click icon to enable VPN");

        return m_networkModel->vpns()[index.row()].value("Id").toString();
    }
    case VpnUuidRole:           return m_networkModel->vpns()[index.row()].value("Uuid").toString();
    case VpnShowIconRole:       return m_activedVpns.contains(m_networkModel->vpns()[index.row()].value("Uuid").toString());
    case VpnIconRole:           return m_hoveredIndex == index ? m_cancelPixmap : m_connectedPixmap;
    case VpnItemHoveredRole:    return m_hoveredIndex == index;
    case Qt::SizeHintRole:      return QSize(0, 36);
    case VpnIsFirstLineRole:    return !index.row();
    case VpnNextRole:           return index.row() == m_hoveredIndex.row() + 1;
    case VpnDisableRole:        return  m_networkModel->vpnEnabled() ? false : true;
    default:;
    }

    return QVariant();
}

void VpnListModel::setHoveredIndex(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_hoveredIndex;

    m_hoveredIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_hoveredIndex, m_hoveredIndex);
}

void VpnListModel::onActivedListChanged(const QSet<QString> &activeConnections)
{
    m_activedVpns.clear();
    m_activedVpns = activeConnections.toList();

    emit layoutChanged();
}
