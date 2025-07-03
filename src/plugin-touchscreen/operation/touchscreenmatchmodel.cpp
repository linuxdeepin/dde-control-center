// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "touchscreenmatchmodel.h"
#include "touchscreenmodel.h"

TouchScreenMatchModel::TouchScreenMatchModel(TouchScreenModel *parent)
    : QAbstractItemModel{ parent }
    , m_touchScreenModel(parent)
{
    init();
}

QHash<int, QByteArray> TouchScreenMatchModel::roleNames() const
{
    QHash<int, QByteArray> names= QAbstractItemModel::roleNames();
    names[IdRole] = "id";
    names[NameRole] = "name";
    names[DeviceNodeRole] = "deviceNode";
    names[SerialNumberRole] = "serialNumber";
    names[UUIDRole] = "UUID";
    names[ScreenNameRole] = "screenName";
    return names;
}

QModelIndex TouchScreenMatchModel::index(int row, int column, const QModelIndex &) const
{
    if (row < 0 || row >= m_touchScreenList.size()) {
        return QModelIndex();
    }
    return createIndex(row, column);
}

QModelIndex TouchScreenMatchModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

int TouchScreenMatchModel::rowCount(const QModelIndex &) const
{
    return m_touchScreenList.size();
}

int TouchScreenMatchModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant TouchScreenMatchModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_touchScreenList.size()) {
        return QVariant();
    }
qWarning()<<__FUNCTION__<<__LINE__<<index.row()<<role;
    switch (role) {
    case Qt::DisplayRole:
        return QString::number(index.row());//m_touchScreenList.at(index.row()).second;
    case IdRole:
        return m_touchScreenList.at(index.row()).first.id;
    case NameRole:
        return m_touchScreenList.at(index.row()).first.name;
    case DeviceNodeRole:
        return m_touchScreenList.at(index.row()).first.deviceNode;
    case SerialNumberRole:
        return m_touchScreenList.at(index.row()).first.serialNumber;
    case UUIDRole:
        return m_touchScreenList.at(index.row()).first.UUID;
    case ScreenNameRole:
        return m_touchScreenList.at(index.row()).second;
    default:
        break;
    }
    return QVariant();
}

void TouchScreenMatchModel::init()
{
    connect(m_touchScreenModel, &TouchScreenModel::touchMapChanged, this, &TouchScreenMatchModel::resetItems);
    connect(m_touchScreenModel, &TouchScreenModel::touchScreenListChanged, this, &TouchScreenMatchModel::resetItems);
    connect(m_touchScreenModel, &TouchScreenModel::monitorsChanged, this, &TouchScreenMatchModel::resetItems);
    resetItems();
}

void TouchScreenMatchModel::resetItems()
{
    beginResetModel();
    m_touchScreenList.clear();
    const TouchscreenMap touchMap = m_touchScreenModel->touchMap();
    for (const auto &touchscreen : m_touchScreenModel->touchScreenList()) {
        if (touchMap.contains(touchscreen.UUID))
            m_touchScreenList.append(QPair<TouchscreenInfo_V2, QString>(touchscreen, touchMap[touchscreen.UUID]));
        else
            m_touchScreenList.append(QPair<TouchscreenInfo_V2, QString>(touchscreen, QString()));
    }
    endResetModel();
}
