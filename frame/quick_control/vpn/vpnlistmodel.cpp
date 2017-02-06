#include "vpnlistmodel.h"

#include "network/networkmodel.h"

#include <QDebug>

using dcc::network::NetworkModel;

VpnListModel::VpnListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_networkModel(model)
{
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
}

int VpnListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_networkModel->vpns().size();
}

QVariant VpnListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:   return m_networkModel->vpns()[index.row()].value("Id").toString();   break;
    default:;
    }

    return QVariant();
}
