#include "navgationmodel.h"

#include <QSize>
#include <QDebug>

namespace dcc {

NavgationModel::NavgationModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void NavgationModel::insertItem(const QString &item)
{
//    if (idx < m_items.size() && m_items[idx] == item)
//        return;

//    Q_ASSERT(!m_items.contains(item));

    if (m_items.contains(item))
        return;

    int idx = 0;
    for (const auto it : m_allItems)
    {
        if (it == item)
            break;
        if (m_items.contains(it))
            ++idx;
    }

    beginInsertRows(QModelIndex(), idx, idx);
    m_items.insert(idx, item);
    endInsertRows();
}

void NavgationModel::removeItem(const QString &item)
{
    const int idx = m_items.indexOf(item);

    if (idx == -1)
        return;

    beginRemoveRows(QModelIndex(), idx, idx);
    m_items.removeAt(idx);
    endRemoveRows();
}

void NavgationModel::appendAvailableItem(const QString &item)
{
    m_allItems.append(item);
}

void NavgationModel::setCurrentItem(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_currentIndex;

    m_currentIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(m_currentIndex, m_currentIndex);
}

int NavgationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_items.size();
}

QVariant NavgationModel::data(const QModelIndex &index, int role) const
{
    if (m_items.size() <= index.row())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:   return m_items[index.row()];
    case Qt::SizeHintRole:  return QSize(110, 90);
    case ItemHoveredRole:   return index == m_currentIndex;
    default:;
    }

    return QVariant();
}

}
