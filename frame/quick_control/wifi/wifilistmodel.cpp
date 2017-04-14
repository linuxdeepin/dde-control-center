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

      m_networkModel(model),
      m_refershTimer(new QTimer(this))
{
    m_refershTimer->setSingleShot(false);
    m_refershTimer->setInterval(1000 / 60);

    connect(m_refershTimer, &QTimer::timeout, this, &WifiListModel::refershActivatingIndex);
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

    // +1 for "connect to hidden network"
    return count + 1;
}

QVariant WifiListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const ItemInfo info = indexInfo(index.row());

    switch (role) {
    case Qt::DisplayRole:
    {
        if (rowCount(QModelIndex()) == 1)
            return tr("Click icon to enable WIFI");

        if (!info.info && info.device)
            return deviceName(info.device);
        else if (!info.device)
            return tr("Connect to hidden network");
        else
            return info.info->value("Ssid");
    }
    case Qt::SizeHintRole:
        if (rowCount(QModelIndex()) == 1)
            return QSize(0, 36);
        if (!info.info)
            return QSize(0, 24);
        else
            return QSize(0, 36);
    case ItemInfoRole:
        return info.info ? *info.info : QVariant();
    case ItemHoveredRole:
        return index == m_currentIndex;
    case ItemIsHeaderRole:
        return !info.info && info.device;
    case ItemIsActiveRole:
        return info.info && static_cast<const WirelessDevice *>(info.device)->activeApName() == info.info->value("Ssid").toString();
    case ItemIsActivatingRole:
        return m_refershTimer->isActive() && index == m_activatingIndex;
    case ItemDevicePathRole:
        return info.device->path();
    case ItemApPathRole:
        return info.info ? info.info->value("Path") : QVariant();
    case ItemUuidRole:
        return info.info ? m_networkModel->connectionUuidByApInfo(info.device->hwAddr(), info.info->value("Ssid").toString()) : QVariant();
    case ItemIsHiddenTipsRole:
        return !info.info && !info.device;
    case ItemNextRole:
        return m_currentIndex.row() + 1 == index.row();
    case ItemIsPowerOffRole:
        return rowCount(QModelIndex()) == 1;
    case ItemCountRole:
        return m_apInfoList.keys().count();
    default:;
    }

    return QVariant();
}

void WifiListModel::setCurrentHovered(const QModelIndex &index)
{
    m_currentIndex = index;

    emit dataChanged(m_currentIndex, m_currentIndex);
}

void WifiListModel::setCurrentActivating(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_activatingIndex;

    m_activatingIndex = index;

    emit dataChanged(oldIndex, oldIndex);
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

    info.device = nullptr;

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

        connect(d, &WirelessDevice::enableChanged, this, &WifiListModel::onDeviceEnableChanged, Qt::UniqueConnection);
        connect(d, &WirelessDevice::activeApChanged, this, &WifiListModel::refershActivatingIndex, Qt::UniqueConnection);
        connect(d, &WirelessDevice::apAdded, this, &WifiListModel::onDeviceApAdded, Qt::UniqueConnection);
        connect(d, &WirelessDevice::activeApChanged, this, &WifiListModel::onDeviceActiveApChanged, Qt::UniqueConnection);
        connect(d, static_cast<void (WirelessDevice::*)(const NetworkDevice::DeviceStatus) const>(&WirelessDevice::statusChanged), this, &WifiListModel::onDeviceStateChanged, Qt::UniqueConnection);
        connect(d, &WirelessDevice::apRemoved, d, [=](const QString &ssid) { onDeviceApRemoved(d, ssid); }, Qt::UniqueConnection);

        emit requestDeviceApList(d->path());
    }

    // removed devices
    QList<WirelessDevice *> removedDeviceList;
    for (auto it(m_apInfoList.cbegin()); it != m_apInfoList.cend(); ++it)
        if (!devices.contains(it.key()))
            removedDeviceList.append(it.key());
    for (auto *d : removedDeviceList)
        m_apInfoList.remove(d);

    emit layoutChanged();
}

void WifiListModel::onDeviceApAdded(const QJsonObject &info)
{
    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);

    if (!dev->enabled())
        return;

    const int row = indexOf(static_cast<WirelessDevice *>(dev));

    beginInsertRows(QModelIndex(), row, row);
    if (info.value("Ssid").toString() == dev->activeApName())
        m_apInfoList[dev].insert(0, info);
    else
        m_apInfoList[dev].append(info);
    endInsertRows();
}

void WifiListModel::onDeviceApRemoved(dcc::network::WirelessDevice *dev, const QString &ssid)
{
    // XXX: fix if ssid is in used by multiple AP
    int row = indexOf(dev);

    const auto list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Ssid").toString() == ssid)
        {
            row += i + 1;
            beginRemoveRows(QModelIndex(), row, row);
            m_apInfoList[dev].removeAt(i);
            endRemoveRows();
            return;
        }
    }
}

void WifiListModel::onDeviceStateChanged(const NetworkDevice::DeviceStatus &stat)
{
//    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    if (stat >= NetworkDevice::Prepare && stat < NetworkDevice::Activated)
        m_refershTimer->start();
    else
        m_refershTimer->stop();
}

void WifiListModel::onDeviceActiveApChanged(const QString &oldName, const QString &newName)
{
    Q_UNUSED(oldName);

    WirelessDevice *dev = static_cast<WirelessDevice *>(sender());
    Q_ASSERT(dev);

    const auto list = m_apInfoList[dev];
    for (int i(0); i != list.size(); ++i)
    {
        if (list[i].value("Ssid").toString() == newName)
        {
            // pass if already first line
            if (i)
            {
                const auto info = m_apInfoList[dev][i];
//                beginMoveRows(QModelIndex(), i, i, QModelIndex(), 0);
                m_apInfoList[dev].removeOne(info);
                m_apInfoList[dev].insert(0, info);
//                endMoveRows();
            }

            emit layoutChanged();
            return;
        }
    }
}

void WifiListModel::refershActivatingIndex()
{
    emit dataChanged(m_activatingIndex, m_activatingIndex);
}

void WifiListModel::onDeviceEnableChanged(const bool enable)
{
    WirelessDevice *d = static_cast<WirelessDevice*>(sender());
    Q_ASSERT(d);

    if (enable) {
        const int pos = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), pos, pos);
        m_apInfoList.insert(d, QList<QJsonObject>());
        endInsertRows();

        emit requestDeviceApList(d->path());
    } else {
        const int pos = indexOf(d);
        beginRemoveRows(QModelIndex(), pos, pos + m_apInfoList[d].length());
        m_apInfoList.remove(d);
        endRemoveRows();
    }
}
