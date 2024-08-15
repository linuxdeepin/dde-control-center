//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MODULELISTMODEL_H
#define MODULELISTMODEL_H

#include "interface/moduleobject.h"

#include <QAbstractItemModel>

namespace DCC_NAMESPACE {
class ModuleListModelPrivate;
class ModuleListModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ModuleListModel(ModuleObject *parent = nullptr);
    ~ModuleListModel() override;

    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    DCC_DECLARE_PRIVATE(ModuleListModel)
};
}
#endif // ModuleListModel_H
