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
    if (m_soundDeviceDatas.count() < 1) {
        return;
    }
    for (SoundDeviceData *soundDeviceData : m_soundDeviceDatas) {
        delete soundDeviceData;
    }
    m_ports.clear();
    m_soundDeviceDatas.clear();
}

void SoundDeviceModel::addData(Port *port)
{
    if (m_ports.contains(port)) {
        return;
    }
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    SoundDeviceData *data = new SoundDeviceData;
    data->setName(port->name() + "(" + port->cardName() + ")");
    data->setIschecked(port->isEnabled());
    data->setCardId(port->cardId());
    data->setPortId(port->id());
    m_soundDeviceDatas.append(data);
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

    m_soundDeviceDatas.remove(index);
    endRemoveRows();
    m_ports.remove(index);
}


int SoundDeviceModel::getRowCount()
{
    return m_soundDeviceDatas.count();
}

SoundDeviceData *SoundDeviceModel::getSoundDeviceData(int index)
{
    if (m_soundDeviceDatas.count() < index || index < 0) {
        return nullptr;
    }

    return m_soundDeviceDatas.at(index);
}

void SoundDeviceModel::updateSoundDeviceData(Port *port)
{
    for (int index = 0; index < m_soundDeviceDatas.count(); index++) {
        if (m_soundDeviceDatas[index]->getPortId() == port->id()) {
            m_soundDeviceDatas[index]->setIschecked(port->isEnabled());
            m_soundDeviceDatas[index]->setCardId(port->cardId());
            m_soundDeviceDatas[index]->setName(port->name() + "(" + port->cardName() + ")");
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex, { IsChecked });
            break;
        }
    }
}

int SoundDeviceModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_soundDeviceDatas.count();
}

QVariant SoundDeviceModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_soundDeviceDatas.count())
        return QVariant();

    const SoundDeviceData* soundDeviceData = m_soundDeviceDatas[index.row()];
    if (role == NameRole)
        return soundDeviceData->name();
    else if (role == IsChecked)
        return soundDeviceData->ischecked();
    return QVariant();
}
