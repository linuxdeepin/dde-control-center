// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "soundDeviceModel.h"

SoundDeviceModel::SoundDeviceModel(QObject *parent)
    : QAbstractListModel{ parent }
{
}

void SoundDeviceModel::clearData()
{
    if (m_ports.count() < 1) {
        return;
    }

    m_ports.clear();
}

void SoundDeviceModel::addData(Port *port)
{
    if (m_ports.contains(port)) {
        return;
    }
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_ports.append(port);
    endInsertRows();
}

void SoundDeviceModel::removeData(Port *port)
{
    if (!m_ports.contains(port)) {
        return;
    }
    int index = m_ports.indexOf(port);
    beginRemoveRows(QModelIndex(), index, index);
    m_ports.remove(index);
    endRemoveRows();

}


int SoundDeviceModel::getRowCount()
{
    return m_ports.count();
}

int SoundDeviceModel::getCurrentIndex() const
{
    for (int index = 0; index< m_ports.count(); index++) {
        if (m_ports.at(index)->isActive() && m_ports.at(index)->isEnabled()) {
            return index;
        }
    }

    for (int index = 0; index< m_ports.count(); index++) {
        if (m_ports.at(index)->isEnabled()) {
            return index;
        }
    }

    return 0;
}

Port *SoundDeviceModel::getSoundDeviceData(int index)
{
    if (m_ports.count() < index || index < 0) {
        return nullptr;
    }

    return m_ports.at(index);
}

void SoundDeviceModel::updateSoundDeviceData(Port *port)
{
    for (int index = 0; index < m_ports.count(); index++) {
        if (m_ports[index]->id() == port->id()) {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex, {});
            break;
        }
    }
}

void SoundDeviceModel::updateAllSoundDeviceData()
{
    for (int index = 0; index < m_ports.count(); index++) {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {});
    }
}

int SoundDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_ports.count();
}

QVariant SoundDeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_ports.count())
        return QVariant();

    const Port* port = m_ports[index.row()];
    if (role == NameRole)
        return port->name() + "(" + port->cardName() + ")";
    else if (role == IsEnabled)
        return port->isEnabled();
    else if (role == IsActive)
        return port->isActive();

    return QVariant();
}
