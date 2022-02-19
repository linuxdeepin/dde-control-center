/*
 * Copyright (C) 2011 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
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

#include "touchscreenmodel.h"

using namespace dcc::display;

TouchscreenModel::TouchscreenModel(QObject *parent)
    : QObject(parent)
    , m_mode(-1)
{

}

void TouchscreenModel::setDisplayMode(const int mode)
{
    if (m_mode != mode && mode >= 0 && mode < 5) {
        m_mode = mode;
        Q_EMIT displayModeChanged(m_mode);
    }
}

void TouchscreenModel::monitorAdded(Monitor *mon)
{
    m_monitors.append(mon);

    Q_EMIT monitorListChanged();
}

void TouchscreenModel::monitorRemoved(Monitor *mon)
{
    m_monitors.removeOne(mon);

    Q_EMIT monitorListChanged();
}

void TouchscreenModel::setTouchscreenList(const TouchscreenInfoList_V2 &touchscreenList)
{
    if (touchscreenList == m_touchscreenList)
        return;

    m_touchscreenList = touchscreenList;

    Q_EMIT touchscreenListChanged();
}

void TouchscreenModel::setTouchMap(const TouchscreenMap &touchMap)
{
    if (touchMap == m_touchMap)
        return;

    m_touchMap = touchMap;

    Q_EMIT touchscreenMapChanged();
}