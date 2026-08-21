// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SEARCHMODEL_H
#define SEARCHMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QTimer>

namespace dccV25 {
class DccObject;

class SearchModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SearchModel(QObject *parent = nullptr);

    enum DccSearchRole { SearchUrlRole = Qt::UserRole + 300, SearchPlainTextRole, SearchTextRole, SearchWeightRole, SearchDataRole, SearchMatchScoreRole };

    QHash<int, QByteArray> roleNames() const override;

public Q_SLOTS:
    void addSearchData(DccObject *obj, const QString &text, const QString &url);
    void removeSearchData(const DccObject *obj, const QString &text);
    void doSort();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

protected:
    QTimer *m_timer;
};

} // namespace dccV25
#endif // SEARCHMODEL_H
