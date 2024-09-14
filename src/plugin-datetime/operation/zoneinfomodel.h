// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ZONEINFOMODEL_H
#define ZONEINFOMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace dccV25 {

class ZoneInfoModel : public QAbstractListModel
{
public:
    explicit ZoneInfoModel(QObject *parent = nullptr);
    virtual ~ZoneInfoModel();
    enum ZoneInfoRole {
        SearchTextRole = Qt::UserRole + 1,
        ZoneIdRole,
        CityNameRole
    };

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

} // namespace dccV25
#endif // ZONEINFOMODEL_H
