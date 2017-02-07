#include "wifilistmodel.h"
#include "network/networkdevice.h"

#include <QSize>
#include <QDebug>
#include <QTimer>

using dcc::network::NetworkModel;
using dcc::network::NetworkDevice;
using dcc::network::WirelessDevice;

WifiListModel::WifiListModel(NetworkModel *model, QObject *parent)
    : QAbstractListModel(parent),

      m_networkModel(model)
{
    connect(m_networkModel, &NetworkModel::connectionListChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &WifiListModel::onDeviceListChanged);

    QTimer::singleShot(1, this, [=] { onDeviceListChanged(m_networkModel->devices()); });
}

int WifiListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int count = 0;
    for (const auto &list : m_apInfoList)
        count += list.size() + 1;

    return count;
}

QVariant WifiListModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
    {
        const ItemInfo info = indexInfo(index.row());
        if (!info.info)
            return info.device->path();
        else
            return info.info->value("Ssid");
    }
    case Qt::SizeHintRole:
        return QSize(0, 30);
    default:;
    }

    return QVariant();
}

const ItemInfo WifiListModel::indexInfo(const int index) const
{
    ItemInfo info;
    int r = index;
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        WirelessDevice *d = static_cast<WirelessDevice *>(dev);
        if (!m_apInfoList.contains(d))
            continue;

        info.device = dev;

        if (!r)
            return info;
        else
            --r;

        const int s = m_apInfoList[d].size();
        if (r < s)
        {
            info.info = &m_apInfoList[d][r];
            return info;
        } else {
            r -= s;
        }
    }

    Q_UNREACHABLE();

    return info;
}

void WifiListModel::onDeviceListChanged(const QList<dcc::network::NetworkDevice *> &devices)
{
    for (auto *dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        WirelessDevice *d = static_cast<WirelessDevice *>(dev);

        if (m_apInfoList.contains(d))
            continue;

        m_apInfoList.insert(d, QList<QJsonObject>());

        connect(d, &WirelessDevice::apAdded, this, &WifiListModel::onDeviceApAdded);

        emit requestDeviceApList(d->path());
    }
}

void WifiListModel::onDeviceApAdded(const QJsonObject &info)
{
    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);
    Q_ASSERT(m_apInfoList.contains(dev));

    int pos = 0;
    for (auto *d : m_networkModel->devices())
    {
        if (d->type() != NetworkDevice::Wireless)
            continue;

        pos += m_apInfoList[static_cast<WirelessDevice *>(d)].size() + 1;

        if (d == dev)
            break;
    }

    beginInsertRows(QModelIndex(), pos, pos + 1);
    m_apInfoList[dev].append(info);
    endInsertRows();

    emit layoutChanged();
}
