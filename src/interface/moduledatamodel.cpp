// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "moduledatamodel.h"

#include "interface/moduleobject.h"

#include <dstyleoption.h>
#include <DIconTheme>

#include <QSignalMapper>

#include <algorithm>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE

ModuleDataModel::ModuleDataModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_parentObject(nullptr)
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
    case Qt::DecorationRole: {
        auto icon = data->icon();
        if (icon.type() == QVariant::String) {
            return DIconTheme::findQIcon(icon.toString());
        }
        return data->icon();
    }
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
    flag.setFlag(Qt::ItemIsEnabled, !ModuleObject::IsDisabled(module));
    return flag;
}

void ModuleDataModel::onDataChanged(QObject *obj)
{
    ModuleObject *const module = static_cast<ModuleObject *>(obj);
    if (module->extra() || ModuleObject::IsHidden(module))
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
    if (module->extra() || ModuleObject::IsHidden(module) || m_data.contains(module))
        return;

    int row = 0;
    for (auto &&tmpModule : m_parentObject->childrens()) {
        if (tmpModule == module)
            break;
        if (!tmpModule->extra() && !tmpModule->isHidden())
            row++;
    }
    Q_ASSERT(row <= rowCount(QModelIndex()));
    beginInsertRows(QModelIndex(), row, row);
    m_data.insert(row, module);
    endInsertRows();

    ModuleObject *maxOne =
            *std::max_element(m_data.begin(), m_data.end(), [](ModuleObject *a, ModuleObject *b) {
                return a->displayName().length() < b->displayName().length();
            });
    emit newModuleMaxDislayName(maxOne->displayName());
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

void ModuleDataModel::setModuleObject(ModuleObject *const module)
{
    m_parentObject = module;
    QList<ModuleObject *> datas = m_parentObject->childrens();

    beginResetModel();
    m_data.clear();
    for (ModuleObject *tmpModule : datas) {
        if (!tmpModule->extra() && !ModuleObject::IsHidden(tmpModule))
            m_data.append(tmpModule);
    }
    endResetModel();

    connect(m_parentObject, &ModuleObject::insertedChild, this, &ModuleDataModel::onInsertChild);
    connect(m_parentObject, &ModuleObject::removedChild, this, &ModuleDataModel::onRemovedChild);
    connect(m_parentObject,
            &ModuleObject::childStateChanged,
            this,
            [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
                Q_UNUSED(flag)
                Q_UNUSED(state)
                onDataChanged(tmpChild);
            });
    // emit moduleDataChanged if module data changed(e.g. setBadge)
    connect(m_parentObject, &ModuleObject::moduleDataChanged, this, [this]() {
        if (auto mainModule = dynamic_cast<ModuleObject *>(m_parentObject->parent())) {
            mainModule->moduleDataChanged();
        }
    });
}

QModelIndex ModuleDataModel::index(ModuleObject *module) const
{
    return index(m_data.indexOf(module), 0);
}
