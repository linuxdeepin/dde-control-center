// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "navigationmodel.h"

#include "dccobject.h"

#include <QLoggingCategory>

namespace dccV25 {
// static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.NavigationModel");

enum DccModelRole { NavTypeRole = Qt::UserRole + 300, NavUrlRole };

enum NavType {
    Separator,
    Middle,
    End,
};

NavigationModel::NavigationModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

void NavigationModel::setNavigationObject(const QVector<DccObject *> &objs)
{
    beginResetModel();
    m_data.clear();
    for (auto &&obj : objs) {
        if (!obj->displayName().isEmpty()) {
            m_data.append(obj);
        }
    }
    endResetModel();
}

QHash<int, QByteArray> NavigationModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractItemModel::roleNames();
    names[NavTypeRole] = "type";
    names[NavUrlRole] = "url";
    return names;
}

QModelIndex NavigationModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= rowCount()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex NavigationModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int NavigationModel::rowCount(const QModelIndex &) const
{
    if (m_data.isEmpty())
        return 0;
    return m_data.size() * 2 - 1;
}

int NavigationModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant NavigationModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    if (index.row() & 1) {
        switch (role) {
        case Qt::DisplayRole:
            return " / ";
        case NavTypeRole:
            return Separator;
        default:
            break;
        }
    } else {
        int i = index.row() / 2;
        if (i < 0 || i >= m_data.size()) {
            return QVariant();
        }
        const DccObject *item = m_data.at(i);
        switch (role) {
        case Qt::DisplayRole:
            return item->displayName();
        case NavTypeRole:
            return (i == (m_data.size() - 1)) ? End : Middle;
        case NavUrlRole:
            return item->parentName() + "/" + item->name();
        default:
            break;
        }
    }
    return QVariant();
}
} // namespace dccV25
