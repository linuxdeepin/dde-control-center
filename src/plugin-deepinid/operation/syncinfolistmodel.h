// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SYNCINFOLISTMODEL_H
#define SYNCINFOLISTMODEL_H

#include "utils.h"
#include <QAbstractListModel>

struct SyncItemData
{
    SyncType type;
    QString displayIcon;
    QString displayName;
    QStringList keyList;
    bool isChecked;
};

class SyncInfoListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SyncItemRoles {
        TypeRole = Qt::UserRole + 1,
        DisplayIconRole,
        DisplayNameRole,
        KeyListRole,
        IsCheckedRole,
    };
    Q_ENUM(SyncItemRoles)

    explicit SyncInfoListModel(QObject *parent = nullptr);

    void updateSyncItem(const QString &key, bool enable);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[TypeRole] = "type";
        roles[DisplayIconRole] = "displayIcon";
        roles[DisplayNameRole] = "displayName";
        roles[KeyListRole] = "keyList";
        roles[IsCheckedRole] = "isChecked";
        return roles;
    }

private:
    QList<SyncItemData> m_syncItemList;
};

#endif // SYNCINFOLISTMODEL_H
