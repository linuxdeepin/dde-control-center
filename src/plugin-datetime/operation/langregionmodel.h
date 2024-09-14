//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LANGREGIONMODEL_H
#define LANGREGIONMODEL_H

#include <QAbstractListModel>
#include <QObject>

namespace dccV25 {
class LangRegionModel : public QAbstractListModel
{
public:
    explicit LangRegionModel(QObject *parent = nullptr);

    enum LangItemRole {
        SearchTextRole = Qt::UserRole + 1,
        RegionKeyIdRole,
        LocaleKeyIdRole,
        FirstDayOfWeek,
        ShortDate,
        LongDate,
        ShortTime,
        LongTime,
        Currency,
        Digit,
        PaperSize
    };

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};
}
#endif // LANGREGIONMODEL_H
