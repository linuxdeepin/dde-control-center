// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccmodel.h"

#include "dccobject.h"
#include "dccobject_p.h"

#include <QIcon>
#include <QLoggingCategory>

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.model");
#define DccItemRole (Qt::UserRole + 300)
#define DccPageTypeRole (Qt::UserRole + 301)

DccModel::DccModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_root(nullptr)
{
    qCWarning(dccLog) << __FUNCTION__ << __LINE__ << this;
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
    qCWarning(dccLog) << __FUNCTION__ << __LINE__ << this << root << DccObject::Private::FromObject(root)->getChildren().size();
    connect(m_root, &DccObject::childAboutToBeAdded, this, &DccModel::AboutToAddObject);
    connect(m_root, &DccObject::childAdded, this, &DccModel::addObject);
    connect(m_root, &DccObject::childAboutToBeRemoved, this, &DccModel::AboutToRemoveObject);
    connect(m_root, &DccObject::childRemoved, this, &DccModel::removeObject);
    for (auto &&obj : DccObject::Private::FromObject(m_root)->getChildren()) {
        qCWarning(dccLog) << __FUNCTION__ <<obj;
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
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    // names[Qt::DisplayRole] = "display";
    // names[Qt::StatusTipRole] = "description";
    // names[Qt::DecorationRole] = "decoration";
    names[DccItemRole] = "item";
    names[DccPageTypeRole] = "pageType";
    qCWarning(dccLog) << __FUNCTION__ << __LINE__ << names;
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

int DccModel::rowCount(const QModelIndex &parent) const
{
    // qCWarning(dccLog) << __FUNCTION__ << __LINE__ << parent<<m_root;
    if (!m_root)
        return 0;
    // qCWarning(dccLog) << __FUNCTION__ << __LINE__ << parent<<m_root->getChildren().size();

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
    const DccObject *item = static_cast<const DccObject *>(index.internalPointer());
    switch (role) {
    case DccItemRole:
        return QVariant::fromValue(item);
    case DccPageTypeRole:
        if (item->pageType() < DccObject::UserType && !item->page()) {
            return QVariant::fromValue(DccObject::Menu);
        }
        return QVariant::fromValue(item->pageType());
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
    qWarning() << __FUNCTION__ << __LINE__ << obj << sender();
    if (obj) {
        QModelIndex i = index(obj);
        qWarning() << __FUNCTION__ << __LINE__ << obj << i;
        emit dataChanged(i, i);
    }
}

void DccModel::AboutToAddObject(const DccObject *parent, int pos)
{
    QModelIndex i = index(parent);
    beginInsertRows(i, pos, pos);
}

void DccModel::addObject(const DccObject *child)
{
    endInsertRows();
    connectObject(child);
}

void DccModel::AboutToRemoveObject(const DccObject *parent, int pos)
{
    QModelIndex i = index(parent);
    beginRemoveRows(i, pos, pos);
}

void DccModel::removeObject(const DccObject *child)
{
    endRemoveRows();
    disconnectObject(child);
}

void DccModel::connectObject(const DccObject *obj)
{
    connect(obj, &DccObject::objectNameChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::displayNameChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::descriptionChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::iconChanged, this, &DccModel::updateObject);
    connect(obj, &DccObject::badgeChanged, this, &DccModel::updateObject);
}

void DccModel::disconnectObject(const DccObject *obj) { }
} // namespace dccV25
