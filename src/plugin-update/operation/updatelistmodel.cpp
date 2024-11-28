// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#include "updatelistmodel.h"

UpdateListModel::UpdateListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int UpdateListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return m_updateLists.count();
}

QVariant UpdateListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    UpdateItemData data = m_updateLists[index.row()];

    switch (role) {
    case Title:
        return data.title;
    case TitleDescription:
        return data.titleDescription;
    case UpdateLog:
        return data.updateLog;
    case ReleaseTime:
        return data.releaseTime;
    case Checked:
        return data.checked;
    case UpdateStatus:
        return data.updateStatus;
    default:
        break;
    }
    return QVariant();
}

void UpdateListModel::addUpdateData(const UpdateItemData &itemData)
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    // FIXME: Implement me!
    m_updateLists.append(itemData);
    endInsertRows();
}

bool UpdateListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool UpdateListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}
