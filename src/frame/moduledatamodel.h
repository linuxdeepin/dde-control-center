/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef MODULEDATAMODEL_H
#define MODULEDATAMODEL_H

#include <QAbstractItemModel>
#include "interface/namespace.h"

class QSignalMapper;

DCC_BEGIN_NAMESPACE
class ModuleObject;
class ModuleDataModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ModuleDataModel(QObject *parent = nullptr);

    void setData(ModuleObject *const module);

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public slots:
    void onDataChanged(QObject *obj);
    void onInsertChild(ModuleObject *const module);
    void onRemovedChild(ModuleObject *const module);

private:
    ModuleObject *m_data;
    QSignalMapper *m_signalMapper;
};
DCC_END_NAMESPACE

#endif // MODULEDATAMODEL_H
