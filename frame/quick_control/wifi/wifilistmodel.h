#ifndef WIFILISTMODEL_H
#define WIFILISTMODEL_H

#include "network/networkmodel.h"
#include "network/networkdevice.h"
#include "network/wirelessdevice.h"

#include <QAbstractListModel>

struct ItemInfo
{
    const dcc::network::NetworkDevice *device = nullptr;
    const QJsonObject *info = nullptr;
};

class WifiListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum WifiListRole
    {
        UnusedRole = Qt::UserRole,
        ItemHoveredRole,
        ItemIsHeaderRole,
        ItemInfoRole,
    };

    explicit WifiListModel(dcc::network::NetworkModel *model, QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void setCurrentHovered(const QModelIndex &index);

signals:
    void requestDeviceApList(const QString &devPath) const;

private:
    const ItemInfo indexInfo(const int index) const;
    const QString deviceName(const dcc::network::NetworkDevice *wirelessDevice) const;

    void onDeviceListChanged(const QList<dcc::network::NetworkDevice *> &devices);
    void onDeviceApAdded(const QJsonObject &info);

private:
    dcc::network::NetworkModel *m_networkModel;

    QModelIndex m_currentIndex;

    QMap<dcc::network::WirelessDevice *, QList<QJsonObject>> m_apInfoList;
};

#endif // WIFILISTMODEL_H
