// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "audioservermodel.h"

AudioServerModel::AudioServerModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void AudioServerModel::addData(const AudioServerData &data)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_audioServerDatas.append(data);
    endInsertRows();
}

void AudioServerModel::updateAllData()
{
    for (int index = 0; index < m_audioServerDatas.count(); index++) {
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {});
    }
}

void AudioServerModel::updateCheckedService(const QString &name)
{
    for (int index = 0; index < m_audioServerDatas.count(); index++) {
        AudioServerData &data = m_audioServerDatas[index];
        data.checked = name == data.serverName;
        QModelIndex modelIndex = createIndex(index, 0);
        emit dataChanged(modelIndex, modelIndex, {});
    }
}

int AudioServerModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    return m_audioServerDatas.count();
}

QVariant AudioServerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const AudioServerData &data = m_audioServerDatas.at(index.row());

    switch (role) {
    case NameRole:
        return data.name;
    case IsChecked:
        return data.checked;
    case ServerName:
        return data.serverName;
    default:
        break;
    }

    return QVariant();
}

bool AudioServerModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);

    endInsertRows();
    return true;
}

bool AudioServerModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    endRemoveRows();
    return true;
}
