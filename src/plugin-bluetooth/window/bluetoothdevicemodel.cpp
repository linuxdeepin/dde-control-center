/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "bluetoothdevicemodel.h"

#include "bluetoothadapter.h"

#include <QPainter>
#include <QStyleOptionViewItem>
#include <QModelIndex>
#include <QWidget>
#include <QApplication>

#include <DSpinner>

DWIDGET_USE_NAMESPACE

struct ItemAction
{
    const BluetoothDevice *device;
    DViewItemAction *loadingAction;
    DViewItemAction *textAction;
    DViewItemAction *spaceAction;
    DViewItemAction *iconAction;
    DSpinner *loadingIndicator;
    DViewItemActionList actionList;
    explicit ItemAction(const BluetoothDevice *_device)
        : device(_device)
        , loadingAction(new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false))
        , textAction(new DViewItemAction(Qt::AlignLeft, QSize(), QSize(), true))
        , spaceAction(new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), false))
        , iconAction(new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true))
        , loadingIndicator(nullptr)
    {
        iconAction->setData(static_cast<const void *>(device));
        actionList.append(loadingAction);
        actionList.append(textAction);
        actionList.append(spaceAction);
        actionList.append(iconAction);
    }
    ~ItemAction()
    {
        delete loadingAction;
        delete textAction;
        delete iconAction;
        delete spaceAction;
    }
    Q_DISABLE_COPY(ItemAction)
};

BluetoothDeviceModel::BluetoothDeviceModel(const BluetoothAdapter *adapter, bool paired, QWidget *parent)
    : QAbstractItemModel(parent)
    , m_paired(paired)
    , m_adapter(adapter)
    , m_parent(parent)
    , m_showAnonymous(false)
{
    for (QString id : m_adapter->devicesId()) {
        if (m_adapter->devices().contains(id)) {
            const BluetoothDevice *device = m_adapter->devices()[id];
            addDevice(device);
        }
    }
    connect(adapter, &BluetoothAdapter::deviceAdded, this, &BluetoothDeviceModel::addDevice, Qt::QueuedConnection);
    connect(adapter, &BluetoothAdapter::deviceRemoved, this, &BluetoothDeviceModel::removeDevice, Qt::QueuedConnection);
}

BluetoothDeviceModel::~BluetoothDeviceModel()
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        delete (*it);
    }
}

QModelIndex BluetoothDeviceModel::index(const BluetoothDevice *device)
{
    int row = 0;
    for (auto it = m_data.begin(); it != m_data.end(); ++it, ++row) {
        if ((*it)->device == device) {
            return index(row, 0);
        }
    }
    return QModelIndex();
}
// Basic functionality:
QModelIndex BluetoothDeviceModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row < 0 || row >= m_data.size())
        return QModelIndex();
    return createIndex(row, column, const_cast<BluetoothDevice *>(m_data.at(row)->device));
}
QModelIndex BluetoothDeviceModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return QModelIndex();
}

int BluetoothDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}
int BluetoothDeviceModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant BluetoothDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    const BluetoothDevice *device = m_data.at(row)->device;
    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        return device->alias().isEmpty() ? device->name() : device->alias();
    case Qt::DecorationRole:
        if (!device->deviceType().isEmpty())
            return QIcon::fromTheme(QString("buletooth_%1").arg(device->deviceType()));
        else
            return QIcon::fromTheme(QString("buletooth_other"));
    case Dtk::RightActionListRole:
        return QVariant::fromValue(m_data.at(row)->actionList);

    default:
        return QVariant();
    }
}

bool BluetoothDeviceModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const BluetoothDevice *device = static_cast<const BluetoothDevice *>(index.internalPointer());
        QString devAlias = value.toString();
        QString devName(device->name());
        if (devAlias.isEmpty()) {
            Q_EMIT requestSetDevAlias(device, devName);
        } else {
            Q_EMIT requestSetDevAlias(device, devAlias);
        }
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}

Qt::ItemFlags BluetoothDeviceModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    int row = index.row();
    const BluetoothDevice *device = m_data.at(row)->device;
    if (device && device->deviceType() == "pheadset" && device->state() == BluetoothDevice::StateAvailable)
        flag.setFlag(Qt::ItemIsEnabled, false);
    return flag | Qt::ItemIsEditable;
}

void BluetoothDeviceModel::addDevice(const BluetoothDevice *device)
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if ((*it)->device == device) {
            return;
        }
    }

    connect(device, &BluetoothDevice::pairedChanged, this, &BluetoothDeviceModel::onPairedChanged, Qt::UniqueConnection);
    if (device->paired() != m_paired)
        return;
    connect(device, &BluetoothDevice::nameChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::aliasChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::stateChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::trustedChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);
    connect(device, &BluetoothDevice::connectingChanged, this, &BluetoothDeviceModel::updateData, Qt::UniqueConnection);

    ItemAction *item = new ItemAction(device);
    updateItem(item);
    connect(item->iconAction, &DViewItemAction::triggered, this, [this]() {
        DViewItemAction *action = qobject_cast<DViewItemAction *>(sender());
        for (auto it = m_data.begin(); it != m_data.end(); ++it) {
            if ((*it)->iconAction == action) {
                emit detailClick((*it)->device);
                break;
            }
        }
    });
    m_allData.append(item);
    if (m_showAnonymous || !device->name().isEmpty()) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append(item);
        endInsertRows();
    }
}

void BluetoothDeviceModel::onPairedChanged(const bool &paired)
{
    const BluetoothDevice *device = qobject_cast<const BluetoothDevice *>(sender());
    if (!device)
        return;
    if (paired == m_paired) {
        addDevice(device);
    } else {
        removeDevice(device->id());
    }
}

void BluetoothDeviceModel::updateData()
{
    const BluetoothDevice *device = qobject_cast<const BluetoothDevice *>(sender());
    int row = 0;
    for (auto it = m_data.begin(); it != m_data.end(); ++it, ++row) {
        if ((*it)->device == device) {
            updateItem(*it);
            emit dataChanged(index(row, 0), index(row, 0));
            break;
        }
    }
}

void BluetoothDeviceModel::removeDevice(const QString &deviceId)
{
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if ((*it)->device->id() == deviceId) {
            ItemAction *item = *it;
            m_allData.removeOne(item);
            int row = m_data.indexOf(item);
            if (row != -1) {
                beginRemoveRows(QModelIndex(), row, row);
                m_data.removeOne(item);
                endRemoveRows();
            }
            delete item;
            break;
        }
    }
}

void BluetoothDeviceModel::updateItem(ItemAction *item)
{
    const BluetoothDevice *device = item->device;
    if (device->state() == BluetoothDevice::StateAvailable) {
        if (!item->loadingIndicator) {
            item->loadingIndicator = new DSpinner(m_parent);
            item->loadingIndicator->setFixedSize(24, 24);
            item->loadingAction->setWidget(item->loadingIndicator);
        }
        item->loadingIndicator->start();
        item->loadingIndicator->setVisible(true);
        item->loadingAction->setVisible(true);
        item->textAction->setVisible(false);
    } else {
        switch (device->state()) {
        case BluetoothDevice::StateConnected:
            if (device->connectState()) {
                item->textAction->setText(tr("Connected"));
                if (item->loadingIndicator) {
                    item->loadingIndicator->stop();
                    item->loadingIndicator->hide();
                    item->loadingAction->setVisible(false);
                    item->textAction->setVisible(true);
                }
            }
            break;
        case BluetoothDevice::StateUnavailable:
        case BluetoothDevice::StateDisconnecting: {
            item->textAction->setText(tr("Not connected"));
            if (item->loadingIndicator) {
                item->loadingIndicator->stop();
                item->loadingIndicator->hide();
                item->loadingAction->setVisible(false);
                item->textAction->setVisible(true);
            }
        } break;
        default:
            break;
        }
    }
    if (m_paired) {
        item->iconAction->setIcon(m_parent->style()->standardIcon(QStyle::SP_ArrowRight));
    }
}

void BluetoothDeviceModel::showAnonymous(bool show)
{
    if (m_showAnonymous == show)
        return;
    m_showAnonymous = show;
    beginResetModel();
    m_data.clear();
    for (auto it = m_allData.begin(); it != m_allData.end(); ++it) {
        if (m_showAnonymous || !(*it)->device->name().isEmpty())
            m_data.append(*it);
    }
    endResetModel();
}
