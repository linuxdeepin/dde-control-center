// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccmodel.h"

#include "dccobject.h"
#include "dccobject_p.h"

#include <QIcon>
#include <QLoggingCategory>

namespace dccV25 {
// static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.model");

enum DccModelRole {
    DccItemRole = Qt::UserRole + 300,
    DccPageTypeRole,
    DccViewItemPositionRole,
};

enum DccItemPostition {
    Invalid,
    Beginning,
    Middle,
    End,
    OnlyOne,
};

DccModel::DccModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root(nullptr)
{
}

DccModel::~DccModel() { }

DccObject *DccModel::root() const
{
    return m_root;
}

void DccModel::setRoot(DccObject *root)
{
    beginResetModel();
    m_root = root;
    endResetModel();
    Q_EMIT rootChanged(m_root);
    if (!m_root)
        return;
    connect(m_root, &DccObject::childAboutToBeAdded, this, &DccModel::AboutToAddObject);
    connect(m_root, &DccObject::childAdded, this, &DccModel::addObject);
    connect(m_root, &DccObject::childAboutToBeRemoved, this, &DccModel::AboutToRemoveObject);
    connect(m_root, &DccObject::childRemoved, this, &DccModel::removeObject);
    connect(m_root, &DccObject::childAboutToBeMoved, this, &DccModel::AboutToMoveObject);
    connect(m_root, &DccObject::childMoved, this, &DccModel::moveObject);
    for (auto &&obj : DccObject::Private::FromObject(m_root)->getChildren()) {
        connectObject(obj);
    }
}

DccObject *DccModel::getObject(int row)
{
    if (row < 0 || row >= m_root->getChildren().size()) {
        return nullptr;
    }
    return m_root->getChildren().at(row);
}

QHash<int, QByteArray> DccModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[Qt::DisplayRole] = "display";
    names[Qt::StatusTipRole] = "description";
    names[Qt::DecorationRole] = "decoration";
    names[DccItemRole] = "item";
    names[DccPageTypeRole] = "pageType";
    names[DccViewItemPositionRole] = "position";
    return names;
}

QModelIndex DccModel::index(const DccObject *object)
{
    if (object == m_root) {
        return QModelIndex();
    }

    DccObject *parent = DccObject::Private::FromObject(object)->getParent();
    if (!parent) {
        // DGM: actually, it can happen (for instance if the entity is displayed in the local DB of a 3D view)
        // ccLog::Error(QString("An error occurred while creating DB tree index: object '%1' has no parent").arg(object->getName()));
        return QModelIndex();
    }

    int pos = DccObject::Private::FromObject(parent)->getChildIndex(object);
    assert(pos >= 0);

    return createIndex(pos, 0, (void *)object);
}

QModelIndex DccModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    // qWarning() << __FUNCTION__ << __LINE__ << row << column << parentIndex;
    if (row < 0 || row >= m_root->getChildren().size()) {
        return QModelIndex();
    }
    return createIndex(row, column, (void *)m_root->getChildren().at(row));

    if (!hasIndex(row, column, parentIndex)) {
        return QModelIndex();
    }
    DccObject *parent = (parentIndex.isValid() ? static_cast<DccObject *>(parentIndex.internalPointer()) : m_root);
    const DccObject *child = DccObject::Private::FromObject(parent)->getChild(row);
    return child ? createIndex(row, column, (void *)child) : QModelIndex();
}

QModelIndex DccModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }
    DccObject *childItem = static_cast<DccObject *>(index.internalPointer());
    if (!childItem) {
        assert(false);
        return QModelIndex();
    }
    DccObject *parentItem = DccObject::Private::FromObject(childItem)->getParent();

    assert(parentItem);
    if (!parentItem || parentItem == m_root) {
        return QModelIndex();
    }

    return createIndex(DccObject::Private::FromObject(parentItem)->getIndex(), 0, parentItem);
}

int DccModel::rowCount(const QModelIndex &) const
{
    if (!m_root)
        return 0;

    return DccObject::Private::FromObject(m_root)->getChildren().size();
}

int DccModel::columnCount(const QModelIndex &parent) const
{
    // qCWarning(dccLog) << __FUNCTION__ << __LINE__;
    if (!parent.isValid())
        return 0;

    return 1;
}

QVariant DccModel::data(const QModelIndex &index, int role) const
{
    // qCWarning(dccLog) << __FUNCTION__ << index << role;
    if (!index.isValid())
        return QVariant();
    DccObject *item = static_cast<DccObject *>(index.internalPointer());
    switch (role) {
    case DccItemRole:
        return QVariant::fromValue(item);
    case DccPageTypeRole:
        return QVariant::fromValue(item->pageType());
    case DccViewItemPositionRole: {
        int count = rowCount();
        if (index.row() == 0) {
            return count == 1 ? OnlyOne : Beginning;
        } else if (index.row() == count - 1) {
            return End;
        } else {
            return Middle;
        }
    }
        return Invalid;
    case Qt::DisplayRole:
        return item->displayName();
    case Qt::StatusTipRole:
        return item->description();
    case Qt::DecorationRole:
        return QIcon::fromTheme(item->icon());
    }
    return QVariant();
}

void DccModel::updateObject()
{
    DccObject *obj = qobject_cast<DccObject *>(sender());
    if (obj) {
        QModelIndex i = index(obj);
        emit dataChanged(i, i);
    }
}

void DccModel::AboutToAddObject(const DccObject *, int pos)
{
    beginInsertRows(QModelIndex(), pos, pos);
}

void DccModel::addObject(const DccObject *child)
{
    endInsertRows();
    connectObject(child);
}

void DccModel::AboutToRemoveObject(const DccObject *, int pos)
{
    beginRemoveRows(QModelIndex(), pos, pos);
}

void DccModel::removeObject(const DccObject *child)
{
    endRemoveRows();
    disconnectObject(child);
}

void DccModel::AboutToMoveObject(const DccObject *, int pos, int oldPos)
{
    beginMoveRows(QModelIndex(), oldPos, oldPos, QModelIndex(), pos);
}

void DccModel::moveObject(const DccObject *)
{
    endMoveRows();
}

void DccModel::connectObject(const DccObject *obj)
{
    connect(obj, &DccObject::objectNameChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::displayNameChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::descriptionChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::iconChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::badgeChanged, this, &DccModel::updateObject);
}

void DccModel::disconnectObject(const DccObject *obj)
{
    disconnect(obj, nullptr, this, nullptr);
}
} // namespace dccV25
