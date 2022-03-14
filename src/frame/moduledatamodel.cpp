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
#include "listview.h"

#include <QSignalMapper>

DCC_USE_NAMESPACE

ModuleDataModel::ModuleDataModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_data(nullptr)
    , m_signalMapper(nullptr)
{
}

QModelIndex ModuleDataModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!m_data || row < 0 || row >= m_data->getChildrenNumber())
        return QModelIndex();
    return createIndex(row, column, m_data->children(row));
}

QModelIndex ModuleDataModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int ModuleDataModel::rowCount(const QModelIndex &parent) const
{
    if (m_data || !parent.isValid())
        return m_data->getChildrenNumber();

    return 0;
}

int ModuleDataModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ModuleDataModel::data(const QModelIndex &index, int role) const
{
    if (!m_data || !index.isValid())
        return QVariant();

    int row = index.row();
    ModuleObject *data = m_data->children(row);
    switch (role) {
    case Qt::DisplayRole:
        return data->moduleData()->DisplayName;
    case Qt::DecorationRole:
        return data->moduleData()->Icon;
    case Qt::StatusTipRole:
        return data->moduleData()->Description;
    case MessageNumRole:
        return 0;
    default:
        break;
    }
    return QVariant();
}

void ModuleDataModel::onDataChanged(QObject *obj)
{
    int row = m_data->findChild(static_cast<ModuleObject *const>(obj));
    if (row >= 0 && row < m_data->getChildrenNumber()) {
        QModelIndex i = index(row, 0);
        emit dataChanged(i, i);
    }
}

void ModuleDataModel::onInsertChild(ModuleObject *const module)
{
    int row = m_data->findChild(module);
    if (row >= 0 && row < m_data->getChildrenNumber()) {
        beginInsertRows(QModelIndex(), row, row);
        endInsertRows();
    }
}

void ModuleDataModel::onRemovedChild(ModuleObject *const module)
{
    int row = m_data->findChild(module);
    if (row >= 0 && row < m_data->getChildrenNumber()) {
        beginRemoveRows(QModelIndex(), row, row);
        endRemoveRows();
    }
}

void ModuleDataModel::setData(ModuleObject *const module)
{
    beginResetModel();
    m_data = module;
    endResetModel();
    if (m_signalMapper) {
        delete m_signalMapper;
    }
    m_signalMapper = new QSignalMapper(this);
    QList<ModuleObject *> datas = m_data->childrens();
    for (ModuleObject *data : datas) {
        connect(data, SIGNAL(moduleDataChanged(ModuleData *)), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(data, data);
    }
    connect(m_signalMapper, SIGNAL(mapped(QObject *)), this, SLOT(onDataChanged(QObject *)));

    connect(m_data, &ModuleObject::appendedChild, this, &ModuleDataModel::onInsertChild);
    connect(m_data, &ModuleObject::insertedChild, this, &ModuleDataModel::onInsertChild);
    connect(m_data, &ModuleObject::removedChild, this, &ModuleDataModel::onInsertChild);
}
