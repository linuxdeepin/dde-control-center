//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LAYOUTSMODEL_H
#define LAYOUTSMODEL_H

#include <QAbstractListModel>
#include <QSortFilterProxyModel>

namespace dccV25 {

class LayoutsModel : public QAbstractListModel
{
public:
    explicit LayoutsModel(QObject *parent = nullptr);

    enum LayoutRole {
        SearchedTextRole = Qt::UserRole + 1,
        IdRole,
        SectionRole
    };

    // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
};

class LayoutFilterModel : public QSortFilterProxyModel {
public:
    explicit LayoutFilterModel(QObject *parent = nullptr);
    virtual bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
};

}
#endif // LAYOUTSMODEL_H
