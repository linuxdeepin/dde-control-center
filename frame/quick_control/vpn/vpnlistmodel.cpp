#include "vpnlistmodel.h"

#include "network/networkmodel.h"

#include <QDebug>
#include <QSize>

using dcc::network::NetworkModel;

VpnListModel::VpnListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_networkModel(model)
{
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::vpnEnabledChanged, [this] { emit layoutChanged(); });
}

int VpnListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    if (m_networkModel->vpnEnabled())
        return m_networkModel->vpns().size();
    else
        return 0;
}

QVariant VpnListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:   return m_networkModel->vpns()[index.row()].value("Id").toString();
    case Qt::SizeHintRole:  return QSize(0, 30);
    default:;
    }

    return QVariant();
}
