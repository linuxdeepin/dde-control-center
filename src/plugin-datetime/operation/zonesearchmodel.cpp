// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "zonesearchmodel.h"

// #include "dccobject.h"
// #include <DPinyin>
// #include <QIcon>

namespace dccV25 {

ZoneSearchModel::ZoneSearchModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    // setFilterRole(SearchTextRole);
    // setSourceModel(new SearchSourceModel(this));
}

QHash<int, QByteArray> ZoneSearchModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    // names[SearchUrlRole] = "url";
    return names;
}

// QVariant ZoneSearchModel::data(const QModelIndex &index, int role) const
// {
//     return QSortFilterProxyModel::data(index, role);
// }

void ZoneSearchModel::addSearchData(const QString &text)
{
    // static_cast<SearchSourceModel *>(sourceModel())->addSearchData(obj, text, url);
}

void ZoneSearchModel::removeSearchData(const QString &text)
{
    // static_cast<SearchSourceModel *>(sourceModel())->removeSearchData(obj, text);
}

bool ZoneSearchModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    if (!sourceIndex.isValid())
        return false;

    return true;
}

} // namespace dccV25
