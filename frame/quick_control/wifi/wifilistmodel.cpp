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
    connect(m_networkModel, &NetworkModel::deviceEnableChanged, [this] { emit layoutChanged(); });
    connect(m_networkModel, &NetworkModel::deviceListChanged, this, &WifiListModel::onDeviceListChanged);

    QTimer::singleShot(1, this, [=] { onDeviceListChanged(m_networkModel->devices()); });
}

int WifiListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    int count = 0;
    for (auto it(m_apInfoList.cbegin()); it != m_apInfoList.cend(); ++it)
    {
        if (!it.key()->enabled())
            continue;
        count += it.value().size() + 1;
    }

    return count;
}

QVariant WifiListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (rowCount(QModelIndex()) <= index.row())
        return QVariant();

    const ItemInfo info = indexInfo(index.row());

    switch (role) {
    case Qt::DisplayRole:
    {
        if (!info.info)
            return deviceName(info.device);
        else
            return info.info->value("Ssid");
    }
    case Qt::SizeHintRole:
        if (!info.info)
            return QSize(0, 20);
        else
            return QSize(0, 30);
    case ItemInfoRole:
    {
        Q_ASSERT(info.info);
        return *info.info;
    }
    case ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return info.info == nullptr;
    case ItemIsActiveRole:
        return info.info && static_cast<const WirelessDevice *>(info.device)->activeApName() == info.info->value("Ssid").toString();
    case ItemDevicePathRole:
        return info.device->path();
    case ItemApPathRole:
        return info.info->value("Path");
    case ItemUuidRole:
        return m_networkModel->connectionUuidByApInfo(info.device->hwAddr(), info.info->value("Ssid").toString());
    default:;
    }

    return QVariant();
}

void WifiListModel::setCurrentHovered(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_currentIndex;

    m_currentIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_currentIndex, m_currentIndex);
}

int WifiListModel::indexOf(dcc::network::WirelessDevice * const dev) const
{
    int pos = 0;
    for (auto *d : m_networkModel->devices())
    {
        if (d->type() != NetworkDevice::Wireless || !d->enabled())
            continue;

        pos += m_apInfoList[static_cast<WirelessDevice *>(d)].size() + 1;

        if (d == dev)
            break;
    }

    return pos;
}

const ItemInfo WifiListModel::indexInfo(const int index) const
{
    ItemInfo info;
    int r = index;
    for (auto *dev : m_networkModel->devices())
    {
        if (dev->type() != NetworkDevice::Wireless || !dev->enabled())
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

    qDebug() << index;
    qDebug() << rowCount(QModelIndex());
    for (auto it(m_apInfoList.cbegin()); it != m_apInfoList.cend(); ++it)
    {
        qDebug() << it.key()->path();
        qDebug() << it.value().size();
    }

    Q_UNREACHABLE();

    return info;
}

const QString WifiListModel::deviceName(const NetworkDevice *wirelessDevice) const
{
    int index = 1;
    for (const auto *dev : m_networkModel->devices())
    {
        if (!dev->enabled())
            continue;

        if (dev == wirelessDevice)
            break;

        if (dev->type() == NetworkDevice::Wireless)
            ++index;
    }

    return tr("Wireless Card %1").arg(index);
}

void WifiListModel::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    for (auto *dev : devices)
    {
        if (dev->type() != NetworkDevice::Wireless || !dev->enabled())
            continue;

        WirelessDevice *d = static_cast<WirelessDevice *>(dev);

        if (m_apInfoList.contains(d))
            continue;

        m_apInfoList.insert(d, QList<QJsonObject>());

        connect(d, &WirelessDevice::apAdded, this, &WifiListModel::onDeviceApAdded);
        connect(d, &WirelessDevice::apRemoved, d, [=](const QString &ssid) { onDeviceApRemoved(d, ssid); });

        emit requestDeviceApList(d->path());
    }

    emit layoutChanged();
}

void WifiListModel::onDeviceApAdded(const QJsonObject &info)
{
    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);
    Q_ASSERT(m_apInfoList.contains(dev));

//    const int row = indexOf(static_cast<WirelessDevice *>(dev));

//    beginInsertRows(QModelIndex(), row, row);
    m_apInfoList[dev].append(info);
//    endInsertRows();

    emit layoutChanged();
}

void WifiListModel::onDeviceApRemoved(dcc::network::WirelessDevice *dev, const QString &ssid)
{
    // XXX: fix if ssid is in used by multiple AP
//    int row = indexOf(dev);

    const auto list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Ssid").toString() == ssid)
        {
//            row += i + 1;
//            beginRemoveRows(QModelIndex(), row, row);
            m_apInfoList[dev].removeAt(i);
//            endRemoveRows();

            emit layoutChanged();

            return;
        }
    }
}
