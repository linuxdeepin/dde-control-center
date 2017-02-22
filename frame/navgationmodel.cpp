#include "navgationmodel.h"

#include <QSize>

namespace dcc {

NavgationModel::NavgationModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void NavgationModel::insertItem(const int idx, const QString &item)
{
//    if (idx < m_items.size() && m_items[idx] == item)
//        return;

//    Q_ASSERT(!m_items.contains(item));

    if (m_items.contains(item))
        return;

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
    case Qt::SizeHintRole:  return QSize(100, 90);
    default:;
    }

    return QVariant();
}

}
