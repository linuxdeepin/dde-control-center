// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>

namespace dccV25 {
class DccObject;

class SearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject *parent = nullptr);

    enum DccSearchRole { SearchUrlRole = Qt::UserRole + 300, SearchPlainTextRole, SearchIsEndRole, SearchTextRole, SearchWeightRole, SearchDataRole };

    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void addSearchData(DccObject *obj, const QString &text, const QString &url);
    void removeSearchData(const DccObject *obj, const QString &text);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
};

} // namespace dccV25
#endif // SEARCHMODEL_H
