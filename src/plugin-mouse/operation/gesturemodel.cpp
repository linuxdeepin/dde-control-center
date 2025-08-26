//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "gesturemodel.h"

GestureModel::GestureModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

GestureModel::~GestureModel()
{
    qDeleteAll(m_gestures);
}

bool GestureModel::containsGestures(QString direction, int fingersNum)
{
    for (auto data : m_gestures) {
        if (data->direction() == direction && data->fingersNum() == fingersNum) {
            return true;
        }
    }
    return false;
}

void GestureModel::updateGestureData(const GestureData &data)
{
    for (int i = 0; i < m_gestures.size(); i++) {
        if (data.direction() == m_gestures[i]->direction() && data.fingersNum() == m_gestures[i]->fingersNum()) {
            m_gestures[i]->setActionName(data.actionName());
            QModelIndex modelIndex = createIndex(i, 0);
            emit dataChanged(modelIndex, modelIndex, {});
            return;
        }
    }
}

int GestureModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;

    // FIXME: Implement me!
    return m_gestures.count();
}

QVariant GestureModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() < 0 || index.row() >= m_gestures.count())
        return QVariant();

    GestureData *data = m_gestures[index.row()];

    switch (role) {
        case IconRole:
            return getGesturesIconPath(data);
        case DescriptionRole:
            return getGesturesDec(data);
        case ActionsIndexRole:
            return getGestureActionIndex(data);
        case ActionListRole:
            return getGestureActionNames(data);
        default:
            break;
    }

    // FIXME: Implement me!
    return QVariant();
}

QString GestureModel::getGesturesDec(GestureData *data) const
{
    QString description;
    if (data->fingersNum() == 3) {
        if (data->actionType() == "swipe") {
            if (data->direction() == "up") {
                description += tr("Three-finger up");
            } else if (data->direction() == "down") {
                description += tr("Three-finger down");
            } else if (data->direction() == "left") {
                description += tr("Three-finger left");
            } else if (data->direction() == "right") {
                description += tr("Three-finger right");
            }
        } else if (data->actionType() == "tap") {
            description += tr("Three-finger tap");
        }
    } else if (data->fingersNum() == 4) {
        if (data->actionType() == "swipe") {
            if (data->direction() == "up") {
                description += tr("Four-finger up");
            } else if (data->direction() == "down") {
                description += tr("Four-finger down");
            } else if (data->direction() == "left") {
                description += tr("Four-finger left");
            } else if (data->direction() == "right") {
                description += tr("Four-finger right");
            }
        } else if (data->actionType() == "tap") {
            description += tr("Four-finger tap");
        }
    }

    return description;
}

QString GestureModel::getGesturesIconPath(GestureData *data) const
{
    QString direction = data->direction();
    if (data->direction() == "none") {
        direction = "click";
    }
    return  QString("trackpad_gesture_%1_%2").arg(data->fingersNum()).arg(direction);
}

QVariantList GestureModel::getGestureActionNames(GestureData *data) const
{
    QVariantList gestureActionNames;
    for (auto data : data->actionMaps()) {
        QVariantMap map;
        map["actionText"] = data.second;
        map["actionValue"] = data.first;
        gestureActionNames.append(map);
    }

    return gestureActionNames;
}

int GestureModel::getGestureActionIndex(GestureData *data) const
{
    auto maps = data->actionMaps();
    for (int i = 0; i < maps.size(); i++) {
        if (maps[i].first == data->actionName()) {
            return i;
        }
    }
    return 0;
}

bool GestureModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
    return true;
}

bool GestureModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
    return true;
}

void GestureModel::addGestureData(GestureData *data)
{
    if (m_gestures.contains(data)) {
        delete data;
        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_gestures.append(data);
    endInsertRows();
}

void GestureModel::removeGestureData(GestureData *data)
{
    if (!m_gestures.contains(data)) {
        return;
    }

    int index = m_gestures.indexOf(data);
    beginRemoveRows(QModelIndex(), index, index);
    m_gestures.remove(index);
    endRemoveRows();
    delete data;
}

void GestureModel::updateGestureData(GestureData *data)
{
    for (int index = 0; index < m_gestures.count(); index++) {
        if (m_gestures[index]->fingersNum() == data->fingersNum()
            && m_gestures[index]->direction() == data->direction()) {
            QModelIndex modelIndex = createIndex(index, 0);
            emit dataChanged(modelIndex, modelIndex, {});
            break;
        }
    }
}

GestureData *GestureModel::getGestureData(int index) const
{
    if (index < 0 || index >= m_gestures.count())
        return nullptr;
    return m_gestures[index];
}
