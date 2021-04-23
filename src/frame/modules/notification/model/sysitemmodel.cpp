/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "sysitemmodel.h"

using namespace dcc;
using namespace dcc::notification;

SysItemModel::SysItemModel(QObject *parent)
    : QObject(parent)
    , m_isDisturbMode(false)
    , m_isShowInDock(false)
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

#ifdef USE_TABLET
void SysItemModel::setShowInDock(const bool showInDock)
{
    if (m_isShowInDock == showInDock)
        return;
    m_isShowInDock = showInDock;
    Q_EMIT showInDockChanged(showInDock);
}
#endif

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
    case SHOWICON:
        setShowInDock(var.variant().toBool());
        break;
    }
}

