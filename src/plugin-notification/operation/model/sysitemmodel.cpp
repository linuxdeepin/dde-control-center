//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "sysitemmodel.h"

using namespace DCC_NAMESPACE;

SysItemModel::SysItemModel(QObject *parent)
    : QObject(parent)
    , m_isDisturbMode(false)
    , m_isTimeSlot(false)
    , m_isLockScreen(false)
    , m_timeStart("22:00")
    , m_timeEnd("07:00")
{
}

void SysItemModel::setDisturbMode(const bool disturbMode)
{
    if (m_isDisturbMode == disturbMode)
        return;
    m_isDisturbMode = disturbMode;
    Q_EMIT disturbModeChanged(disturbMode);
}

void SysItemModel::setTimeSlot(const bool timeSlot)
{
    if (m_isTimeSlot == timeSlot)
        return;
    m_isTimeSlot = timeSlot;
    Q_EMIT timeSlotChanged(timeSlot);
}

void SysItemModel::setLockScreen(const bool lockScreen)
{
    if (m_isLockScreen == lockScreen)
        return;
    m_isLockScreen = lockScreen;
    Q_EMIT lockScreenChanged(lockScreen);
}

void SysItemModel::setTimeStart(const QString &timeStart)
{
    if (m_timeStart == timeStart)
        return;
    m_timeStart = timeStart;
    Q_EMIT timeStartChanged(timeStart);
}

void SysItemModel::setTimeEnd(const QString &timeEnd)
{
    if (m_timeEnd == timeEnd)
        return;
    m_timeEnd = timeEnd;
    Q_EMIT timeEndChanged(timeEnd);
}

void SysItemModel::onSettingChanged(uint item, const QDBusVariant &var)
{
    switch (item) {
    case DNDMODE:
        setDisturbMode(var.variant().toBool());
        break;
    case LOCKSCREENOPENDNDMODE:
        setLockScreen(var.variant().toBool());
        break;
    case OPENBYTIMEINTERVAL:
        setTimeSlot(var.variant().toBool());
        break;
    case STARTTIME:
        setTimeStart(var.variant().toString());
        break;
    case ENDTIME:
        setTimeEnd(var.variant().toString());
        break;
    }
}

