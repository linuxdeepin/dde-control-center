//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "widgets/modulelistmodel.h"
#include "widgets/moduleobjectitem.h"

#include <QMap>
#include <QWidget>
#include <QHBoxLayout>

using namespace DCC_NAMESPACE;
namespace DCC_NAMESPACE {
class ModuleListModelPrivate
{
public:
    explicit ModuleListModelPrivate(ModuleListModel *object)
        : q_ptr(object)
        , m_module(nullptr)
    {
    }

    void init(ModuleObject *module)
    {
        m_module = module;
        QObject::connect(m_module, &ModuleObject::insertedChild, q_ptr, [this](ModuleObject *const module) { onInsertChild(module); });
        QObject::connect(m_module, &ModuleObject::removedChild, q_ptr, [this](ModuleObject *const module) { onRemovedChild(module); });
        QObject::connect(m_module, &ModuleObject::childStateChanged, q_ptr, [this](ModuleObject *const tmpChild, uint32_t flag, bool state) {
            onDataChanged(tmpChild, flag, state);
        });
        for (auto &&childModule : m_module->childrens()) {
            QObject::connect(childModule, &ModuleObject::moduleDataChanged, q_ptr, [this]() {
                ModuleObject *module = qobject_cast<ModuleObject *>(q_ptr->sender());
                int row = m_data.indexOf(module);
                QModelIndex i = q_ptr->index(row, 0);
                emit q_ptr->dataChanged(i, i);
            });
        }
    }

    void onInsertChild(ModuleObject *const module)
    {
        if (ModuleObject::IsHidden(module) || m_data.contains(module))
            return;
        QObject::connect(module, &ModuleObject::moduleDataChanged, q_ptr, [this]() {
            ModuleObject *module = qobject_cast<ModuleObject *>(q_ptr->sender());
            int row = m_data.indexOf(module);
            QModelIndex i = q_ptr->index(row, 0);
            emit q_ptr->dataChanged(i, i);
        });
        Q_Q(ModuleListModel);
        int row = 0;
        for (auto &&tmpModule : m_module->childrens()) {
            if (tmpModule == module)
                break;
            row++;
        }
        q->beginInsertRows(QModelIndex(), row, row);
        m_data.insert(row, module);
        q->endInsertRows();
    }
    void onRemovedChild(ModuleObject *const module)
    {
        QObject::disconnect(module, nullptr, q_ptr, nullptr);
        Q_Q(ModuleListModel);
        int row = m_data.indexOf(module);
        if (row >= 0 && row < m_data.size()) {
            q->beginRemoveRows(QModelIndex(), row, row);
            m_data.removeAt(row);
            q->endRemoveRows();
        }
    }
    void onDataChanged(ModuleObject *const module, uint32_t flag, bool state)
    {
        Q_Q(ModuleListModel);
        if (ModuleObject::IsHiddenFlag(flag)) {
            if (state)
                onRemovedChild(module);
            else
                onInsertChild(module);
        }
        int row = m_data.indexOf(module);
        QModelIndex i = q->index(row, 0);
        emit q->dataChanged(i, i);
    }

public:
    ModuleListModel *q_ptr;
    Q_DECLARE_PUBLIC(ModuleListModel)
    ModuleObject *m_module;
    QList<ModuleObject *> m_data;
};
}

ModuleListModel::ModuleListModel(ModuleObject *parent)
    : QAbstractItemModel(parent)
    , DCC_INIT_PRIVATE(ModuleListModel)
{
    Q_D(ModuleListModel);
    d->init(parent);
}

ModuleListModel::~ModuleListModel()
{
}

QModelIndex ModuleListModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    Q_D(const ModuleListModel);
    if (row < 0 || row >= d->m_data.size())
        return QModelIndex();
    return createIndex(row, column, d->m_data.at(row));
}

QModelIndex ModuleListModel::parent(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QModelIndex();
}

int ModuleListModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const ModuleListModel);
    if (!parent.isValid())
        return d->m_data.size();

    return 0;
}

int ModuleListModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant ModuleListModel::data(const QModelIndex &index, int role) const
{
    ModuleObject *module = static_cast<ModuleObject *>(index.internalPointer());
    if (!module)
        return QVariant();

    ModuleObjectItem *item = module->getClassID() == ITEM ? qobject_cast<ModuleObjectItem *>(module) : nullptr;
    if (item)
        return item->data(role);

    switch (role) {
    case Qt::DisplayRole:
        return module->displayName();
    case Qt::DecorationRole:
        return module->icon();
    case Qt::StatusTipRole:
        return module->description();
    default:
        break;
    }
    return QVariant();
}

Qt::ItemFlags ModuleListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractItemModel::flags(index);
    ModuleObject *module = static_cast<ModuleObject *>(index.internalPointer());
    flag.setFlag(Qt::ItemIsEnabled, !ModuleObject::IsDisabled(module));
    return flag;
}
