#ifndef VPNLISTMODEL_H
#define VPNLISTMODEL_H

#include <QAbstractListModel>

namespace dcc {

namespace network {

class NetworkModel;

}

}

class VpnListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit VpnListModel(dcc::network::NetworkModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    dcc::network::NetworkModel *m_networkModel;
};

#endif // VPNLISTMODEL_H
