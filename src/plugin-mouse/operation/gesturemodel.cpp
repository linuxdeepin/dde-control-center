//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
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

bool GestureModel::containsGesture(const QString &gestureId) const
{
    for (const GestureData *data : m_gestures) {
        if (data->gestureId() == gestureId)
            return true;
    }
    return false;
}

void GestureModel::updateGestureData(const GestureData &data)
{
    for (int i = 0; i < m_gestures.size(); i++) {
        if (data.gestureId() == m_gestures[i]->gestureId()) {
            m_gestures[i]->setGestureId(data.gestureId());
            m_gestures[i]->setDisplayName(data.displayName());
            m_gestures[i]->setActionName(data.actionName());
            m_gestures[i]->setActions(data.actions());
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
            return data->displayName();
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
    for (const GestureActionData &action : data->actions()) {
        QVariantMap map;
        map["actionText"] = action.displayName;
        map["actionValue"] = action.actionId;
        map["supported"] = action.supported;
        map["unavailableReason"] = action.unavailableReason;
        gestureActionNames.append(map);
    }

    return gestureActionNames;
}

int GestureModel::getGestureActionIndex(GestureData *data) const
{
    const QList<GestureActionData> actions = data->actions();
    for (int i = 0; i < actions.size(); i++) {
        if (actions[i].actionId == data->actionName()) {
            return i;
        }
    }
    return -1;
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

    int insertPos = m_gestures.size();
    for (int i = 0; i < m_gestures.size(); ++i) {
        if (m_gestures[i]->sequence() > data->sequence()) {
            insertPos = i;
            break;
        }
    }

    beginInsertRows(QModelIndex(), insertPos, insertPos);
    m_gestures.insert(insertPos, data);
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

GestureData *GestureModel::getGestureData(int index) const
{
    if (index < 0 || index >= m_gestures.count())
        return nullptr;
    return m_gestures[index];
}
