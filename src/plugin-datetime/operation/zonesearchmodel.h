// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ZONESEARCHMODEL_H
#define ZONESEARCHMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace dccV25 {

class ZoneSearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ZoneSearchModel(QObject *parent = nullptr);

    enum SearchRole { SearchDataRole = Qt::UserRole + 300 };

    QHash<int, QByteArray> roleNames() const override;
    // QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void addSearchData(const QString &text);
    void removeSearchData(const QString &text);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

} // namespace dccV25
#endif // ZONESEARCHMODEL_H
