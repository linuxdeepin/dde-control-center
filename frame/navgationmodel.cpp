#include "navgationmodel.h"

#include <QSize>

namespace dcc {

NavgationModel::NavgationModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

void NavgationModel::insertItem(const int idx, const QString &item)
{
    beginInsertRows(QModelIndex(), idx, idx);
    m_items.insert(idx, item);
    endInsertRows();
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
