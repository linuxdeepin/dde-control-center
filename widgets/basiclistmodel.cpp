#include "basiclistmodel.h"

#include <QSize>
#include <QDebug>

namespace dcc {

namespace widgets {

BasicListModel::BasicListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int BasicListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_options.size();
}

QVariant BasicListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role) {
    case ItemTextRole:      return m_options[index.row()];
    case ItemSizeRole:      return QSize(0, 36);
    case ItemSelectedRole:  return m_selectedIndex == index;
    case ItemHoverRole:     return m_hoveredIndex == index;
    default:;
    }

    return QVariant();
}

void BasicListModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, m_options.size());
    m_options.clear();
    m_values.clear();
    endRemoveRows();
}

void BasicListModel::appendOption(const QString &text, const QVariant &data)
{
    beginInsertRows(QModelIndex(), m_options.size(), m_options.size());
    m_options.append(text);
    m_values.append(data);
    endInsertRows();

    //    emit layoutChanged();
}

void BasicListModel::setSelectedIndex(const QModelIndex &index)
{
    const QModelIndex oldIndex = m_selectedIndex;

    m_selectedIndex = index;

    emit dataChanged(oldIndex, oldIndex);
    emit dataChanged(index, index);
}

void BasicListModel::setHoveredIndex(const QModelIndex &index)
{
    m_hoveredIndex = index;

    emit dataChanged(index, index);
}

}

}
