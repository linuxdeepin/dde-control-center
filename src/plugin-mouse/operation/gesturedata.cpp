//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
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

QStringList GestureData::actionNameList() const
{
    return m_actionNameList;
}

void GestureData::setActionNameList(const QStringList &newActionNameList)
{
    m_actionNameList = newActionNameList;
}

QStringList GestureData::actionDescriptionList() const
{
    return m_actionDescriptionList;
}

void GestureData::setActionDescriptionList(const QStringList &newActionDescriptionList)
{
    m_actionDescriptionList = newActionDescriptionList;
}

QList<QPair<QString, QString> > GestureData::actionMaps() const
{
    return m_actionMaps;
}

void GestureData::setActionMaps(const QList<QPair<QString, QString> > &newActionMaps)
{
    m_actionMaps = newActionMaps;
}

void GestureData::addActiosPair(const QPair<QString, QString> &actionPair)
{
    m_actionMaps.append(actionPair);
}

GestureData::GestureData(QObject *parent)
    : QObject(parent)
{

}
