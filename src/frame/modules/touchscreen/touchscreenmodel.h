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

#ifndef TOUCHSCREENMODEL_H
#define TOUCHSCREENMODEL_H

#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"

#include <QObject>

namespace dcc {

namespace display {

class Monitor;
class TouchscreenWorker;
class TouchscreenModel : public QObject
{
    Q_OBJECT

public:
    friend class TouchscreenWorker;

public:
    explicit TouchscreenModel(QObject *parent = 0);

    inline int displayMode() const { return m_mode; }
    inline const QList<Monitor *> monitorList() const { return m_monitors; }

    inline TouchscreenInfoList_V2 touchscreenList() const { return m_touchscreenList; }
    void setTouchscreenList(const TouchscreenInfoList_V2 &touchscreenList);

    inline TouchscreenMap touchMap() const { return m_touchMap; }
    void setTouchMap(const TouchscreenMap &touchMap);

Q_SIGNALS:
    void displayModeChanged(const int mode) const;
    void monitorListChanged() const;
    void touchscreenListChanged() const;
    void touchscreenMapChanged() const;

private Q_SLOTS:
    void setDisplayMode(const int mode);
    void monitorAdded(Monitor *mon);
    void monitorRemoved(Monitor *mon);

private:
    int m_mode;
    QList<Monitor *> m_monitors;
    TouchscreenInfoList_V2 m_touchscreenList;
    TouchscreenMap m_touchMap;
};

} // namespace display

} // namespace dcc

#endif // TOUCHSCREENMODEL_H
