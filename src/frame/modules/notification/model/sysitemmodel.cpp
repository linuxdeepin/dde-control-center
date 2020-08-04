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

#include <QJsonObject>

using namespace dcc;
using namespace dcc::notification;

SysItemModel::SysItemModel(QObject *parent)
    : QObject(parent)
    , m_isDisturbMode(false)
    , m_isShowInDock(false)
    , m_isTimeSlot(false)
    , m_isFullScreen(false)
    , m_isProjector(false)
    , m_isLockScreen(false)
    , m_timeStart(QTime::fromString("22:00","hh:mm"))
    , m_timeEnd(QTime::fromString("07:00","hh:mm"))
{
}

void SysItemModel::setItem(const QJsonObject &item)
{
    setFullScreen(item["AppsInFullscreen"].toBool());
    setProjector(item["ConnectedProjector"].toBool());
    setLockScreen(item["ScreenLocked"].toBool());
    setDisturbMode(item["DoNotDisturb"].toBool());
    setShowInDock(item["ShowIconOnDock"].toBool());
    setTimeSlot(item["TimeSlot"].toBool());
    setTimeStart(QTime::fromString(item["StartTime"].toString(), "hh:mm"));
    setTimeEnd(QTime::fromString(item["EndTime"].toString(), "hh:mm"));
}

QJsonObject SysItemModel::convertQJson()
{
    QJsonObject jsonObj;
    jsonObj.insert("AppsInFullscreen", isFullScreen());
    jsonObj.insert("ScreenLocked", isLockScreen());
    jsonObj.insert("DoNotDisturb", isDisturbMode());
    jsonObj.insert("ShowIconOnDock", isShowInDock());
    jsonObj.insert("ConnectedProjector", isProjector());
    jsonObj.insert("EndTime", timeEnd().toString("hh:mm"));
    jsonObj.insert("StartTime", timeStart().toString("hh:mm"));
    jsonObj.insert("TimeSlot", isTimeSlot());
    QJsonObject json_fa;
    json_fa.insert("SystemNotify", jsonObj);
    return json_fa;
}

void SysItemModel::setDisturbMode(const bool disturbMode)
{
    if (m_isDisturbMode != disturbMode) {
        m_isDisturbMode = disturbMode;

        Q_EMIT disturbModeChanged(disturbMode);
    }
}

void SysItemModel::setShowInDock(const bool showInDock)
{
    if (m_isShowInDock != showInDock) {
        m_isShowInDock = showInDock;

        Q_EMIT showInDockChanged(showInDock);
    }
}

void SysItemModel::setTimeSlot(const bool timeSlot)
{
    if (m_isTimeSlot != timeSlot) {
        m_isTimeSlot = timeSlot;

        Q_EMIT timeSlotChanged(timeSlot);
    }
}

void SysItemModel::setFullScreen(const bool fullScreen)
{
    if (m_isFullScreen != fullScreen) {
        m_isFullScreen = fullScreen;

        Q_EMIT fullScreenChanged(fullScreen);
    }
}

void SysItemModel::setProjector(const bool projector)
{
    if (m_isProjector != projector) {
        m_isProjector = projector;

        Q_EMIT projectorChanged(projector);
    }
}

void SysItemModel::setLockScreen(const bool lockScreen)
{
    if (m_isLockScreen != lockScreen) {
        m_isLockScreen = lockScreen;

        Q_EMIT lockScreenChanged(lockScreen);
    }
}

void SysItemModel::setTimeStart(const QTime &timeStart)
{
    if (m_timeStart != timeStart) {
        m_timeStart = timeStart;

        Q_EMIT timeStartChanged(timeStart);
    }
}

void SysItemModel::setTimeEnd(const QTime &timeEnd)
{
    if (m_timeEnd != timeEnd) {
        m_timeEnd = timeEnd;

        Q_EMIT timeEndChanged(timeEnd);
    }
}

