//SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "gesturedata.h"

QString GestureData::actionType() const
{
    return m_actionType;
}

void GestureData::setActionType(const QString &newActionType)
{
    if (m_actionType == newActionType)
        return;
    m_actionType = newActionType;
    emit actionTypeChanged();
}

QString GestureData::gestureId() const
{
    return m_gestureId;
}

void GestureData::setGestureId(const QString &newGestureId)
{
    if (m_gestureId == newGestureId)
        return;
    m_gestureId = newGestureId;
    emit gestureIdChanged();
}

QString GestureData::displayName() const
{
    return m_displayName;
}

void GestureData::setDisplayName(const QString &newDisplayName)
{
    if (m_displayName == newDisplayName)
        return;
    m_displayName = newDisplayName;
    emit displayNameChanged();
}

QString GestureData::direction() const
{
    return m_direction;
}

void GestureData::setDirection(const QString &newDirection)
{
    if (m_direction == newDirection)
        return;
    m_direction = newDirection;
    emit directionChanged();
}

int GestureData::fingersNum() const
{
    return m_fingersNum;
}

void GestureData::setFingersNum(int newFingersNum)
{
    if (m_fingersNum == newFingersNum)
        return;
    m_fingersNum = newFingersNum;
    emit fingersNumChanged();
}

QString GestureData::actionName() const
{
    return m_actionName;
}

void GestureData::setActionName(const QString &newActionName)
{
    if (m_actionName == newActionName)
        return;
    m_actionName = newActionName;
    emit actionNameChanged();
}

int GestureData::sequence() const
{
    return m_sequence;
}

void GestureData::setSequence(int newSequence)
{
    if (m_sequence == newSequence)
        return;
    m_sequence = newSequence;
    emit sequenceChanged();
}

QList<GestureActionData> GestureData::actions() const
{
    return m_actions;
}

void GestureData::setActions(const QList<GestureActionData> &actions)
{
    m_actions = actions;
}

GestureData::GestureData(QObject *parent)
    : QObject(parent)
{

}
