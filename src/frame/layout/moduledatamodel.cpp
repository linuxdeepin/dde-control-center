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
#include "moduledatamodel.h"
#include "interface/moduleobject.h"
#include "interface/layoutbase.h"

#include <dstyleoption.h>

#include <QSignalMapper>

DCC_USE_NAMESPACE

ModuleDataModel::ModuleDataModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_parentObject(nullptr)
    , m_signalMapper(nullptr)
{
}

QModelIndex ModuleDataModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    if (row < 0 || row >= m_data.size())
        return QModelIndex();
    return createIndex(row, column, m_data.at(row));
}

QModelIndex ModuleDataModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ModuleDataModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return m_data.size();

    return 0;
}

int ModuleDataModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ModuleDataModel::data(const QModelIndex &index, int role) const
{
    if (m_data.isEmpty() || !index.isValid())
        return QVariant();

    int row = index.row();
    ModuleObject *data = m_data.at(row);
    switch (role) {
    case Qt::DisplayRole:
        return data->displayName();
    case Qt::DecorationRole:
        return data->icon();
    case Qt::StatusTipRole:
        return data->description();
    case Dtk::RightActionListRole:
        return data->badge();
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ModuleDataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    ModuleObject *module = static_cast<ModuleObject *>(index.internalPointer());
    flag.setFlag(Qt::ItemIsEnabled, LayoutBase::IsEnabled(module));
    return flag;
}

void ModuleDataModel::onDataChanged(QObject *obj)
{
    ModuleObject *const module = static_cast<ModuleObject *const>(obj);
    if (module->extra() || !LayoutBase::IsVisible(module))
        onRemovedChild(module);
    else {
        int row = m_data.indexOf(module);
        if (row >= 0 && row < m_data.size()) {
            QModelIndex i = index(row, 0);
            emit dataChanged(i, i);
        } else {
            onInsertChild(module);
        }
    }
}

void ModuleDataModel::onInsertChild(ModuleObject *const module)
{
    if (module->extra() || !LayoutBase::IsVisible(module))
        return;
    int row = 0;
    for (auto &&tmpModule : m_parentObject->childrens()) {
        if (tmpModule == module)
            break;
        if (!tmpModule->extra())
            row++;
    }
    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row, module);
    endInsertRows();
}

void ModuleDataModel::onRemovedChild(ModuleObject *const module)
{
    int row = m_data.indexOf(module);
    if (row >= 0 && row < m_data.size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_data.removeAt(row);
        endRemoveRows();
    }
}

void ModuleDataModel::setData(ModuleObject *const module)
{
    m_parentObject = module;
    if (m_signalMapper) {
        delete m_signalMapper;
    }
    m_signalMapper = new QSignalMapper(this);
    QList<ModuleObject *> datas = m_parentObject->childrens();

    beginResetModel();
    m_data.clear();
    for (ModuleObject *tmpModule : datas) {
        if (!tmpModule->extra() && LayoutBase::IsVisible(tmpModule))
            m_data.append(tmpModule);
        connect(tmpModule, SIGNAL(moduleDataChanged()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(tmpModule, tmpModule);
    }
    endResetModel();

    connect(m_signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(onDataChanged(QObject *)));

    connect(m_parentObject, &ModuleObject::appendedChild, this, &ModuleDataModel::onInsertChild);
    connect(m_parentObject, &ModuleObject::insertedChild, this, &ModuleDataModel::onInsertChild);
    connect(m_parentObject, &ModuleObject::removedChild, this, &ModuleDataModel::onRemovedChild);
}

QModelIndex ModuleDataModel::index(ModuleObject *module) const
{
    return index(m_data.indexOf(module), 0);
}
