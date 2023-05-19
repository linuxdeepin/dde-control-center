// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MODULEDATAMODEL_H
#define MODULEDATAMODEL_H

#include "interface/namespace.h"

#include <QAbstractItemModel>

class QSignalMapper;

namespace DCC_NAMESPACE {
class ModuleObject;

class ModuleDataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ModuleDataModel(QObject *parent = nullptr);

    void setModuleObject(ModuleObject *const module);
    QModelIndex index(DCC_NAMESPACE::ModuleObject *module) const;
    // Basic functionality:
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
signals:
    Q_DECL_DEPRECATED void newModuleDislayNameLen(int);
    void newModuleMaxDislayName(const QString&);

public slots:
    void onDataChanged(QObject *obj);
    void onInsertChild(ModuleObject *const module);
    void onRemovedChild(ModuleObject *const module);

private:
    QList<ModuleObject *> m_data;
    ModuleObject *m_parentObject;
};
} // namespace DCC_NAMESPACE

#endif // MODULEDATAMODEL_H
