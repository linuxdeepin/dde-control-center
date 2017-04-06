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
