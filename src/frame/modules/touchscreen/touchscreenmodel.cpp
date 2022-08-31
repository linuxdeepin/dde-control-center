// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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