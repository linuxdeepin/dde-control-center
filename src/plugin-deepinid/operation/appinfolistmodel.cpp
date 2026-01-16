// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appinfolistmodel.h"

#include <QUrl>

AppInfoListModel::AppInfoListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

AppInfoListModel::~AppInfoListModel()
{
    clearItem();
}

void AppInfoListModel::addAppItem(AppItemData *item)
{
    beginInsertRows(QModelIndex(), m_appItemList.count(), m_appItemList.count());
    m_appItemList.append(item);
    endInsertRows();
}

void AppInfoListModel::removeAppItem(AppItemData* item)
{
    beginResetModel();
    m_appItemList.removeAll(item);
    delete item;
    item = nullptr;
    endResetModel();
}

void AppInfoListModel::clearItem()
{
    beginResetModel();
    for (AppItemData* item : m_appItemList) {
        delete item;
        item = nullptr;
    }
    m_appItemList.clear();
    endResetModel();
}

void AppInfoListModel::updateAppItem(const QString &key, bool enable)
{
    for (AppItemData* item : m_appItemList) {
        if (item->key == key) {
            item->enable = enable;
            QModelIndex modelIndex = createIndex(m_appItemList.indexOf(item), 0);
            emit dataChanged(modelIndex, modelIndex, { EnableRole });
            return;
        }
    }
}

int AppInfoListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_appItemList.count();
}

QVariant AppInfoListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_appItemList.count())
        return QVariant();

    auto appItem = m_appItemList[index.row()];
    switch (role) {
        case NameRole:
            return appItem->name;
        case IconRole:
            return appItem->icon.startsWith("/") ? QUrl::fromLocalFile(appItem->icon).toString() : appItem->icon;
        case KeyRole:
            return appItem->key;
        case EnableRole:
            return appItem->enable;
        default:
            break;
    }

    return QVariant();
}
