// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef APPSLISTMODEL_H
#define APPSLISTMODEL_H

#include <QObject>
#include <QSortFilterProxyModel>

namespace DCC_NAMESPACE{

class AppsListModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AppsListModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};
}
#endif // APPSLISTMODEL_H
