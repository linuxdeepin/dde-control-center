// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "syncinfolistmodel.h"

#include <QUrl>

SyncInfoListModel::SyncInfoListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_syncItemList = {
        { Sound, "dcc_cloud_voice", tr("Sound"), QStringList() << "audio", false },
        { Power, "dcc_cloud_power", tr("Power"), QStringList() << "power", false },
        { Mouse, "dcc_cloud_mouse", tr("Mouse"), QStringList() << "peripherals", false },
        { Update, "dcc_cloud_update", tr("Update"), QStringList() << "updater", false },
        { Screensaver, "dcc_cloud_wallpaper", tr("Screensaver"), QStringList() << "screensaver", false },
    };
}

void SyncInfoListModel::updateSyncItem(const QString &key, bool enable)
{
    for (int i = 0; i < m_syncItemList.count(); i++) {
        auto &syncItem = m_syncItemList[i];
        if (syncItem.keyList.contains(key)) {
            syncItem.isChecked = enable;
            QModelIndex modelIndex = createIndex(i, 0);
            emit dataChanged(modelIndex, modelIndex, { IsCheckedRole });
            break;
        }
    }
}

int SyncInfoListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_syncItemList.count();
}

QVariant SyncInfoListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_syncItemList.count())
        return QVariant();

    auto syncItem = m_syncItemList[index.row()];
    switch (role) {
        case TypeRole:
            return syncItem.type;
        case DisplayIconRole:
            return syncItem.displayIcon.startsWith("/") ? QUrl::fromLocalFile(syncItem.displayIcon).toString() : syncItem.displayIcon;
        case DisplayNameRole:
            return syncItem.displayName;
        case KeyListRole:
            return syncItem.keyList;
        case IsCheckedRole:
            return syncItem.isChecked;
        default:
            break;
    }

    return QVariant();
}
