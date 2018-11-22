/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef MONITORSGROUND_H
#define MONITORSGROUND_H

#include "monitor.h"

#include <QFrame>

namespace dcc {

namespace display {

class DisplayModel;
class MonitorProxyWidget;
class MonitorsGround : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorsGround(QWidget *parent = 0);
    ~MonitorsGround();

    void setDisplayModel(DisplayModel *model);

Q_SIGNALS:
    void requestApplySettings(Monitor *mon, const int x, const int y);
    void requestMonitorPress(Monitor *mon);
    void requestMonitorRelease(Monitor *mon);

private Q_SLOTS:
    void resetMonitorsView();
    void monitorMoved(MonitorProxyWidget *pw);
    void adjust(MonitorProxyWidget *pw);

private:
    void ensureWidgetPerfect(MonitorProxyWidget *pw);
    void reloadViewPortSize();
    void applySettings();
    bool isScreenPerfect() const;
    double screenScale() const;
    const QPoint bestMoveOffset(MonitorProxyWidget *pw0, MonitorProxyWidget *pw1) const;

private:
    int m_viewPortWidth;
    int m_viewPortHeight;
    DisplayModel *m_model;
    QMap<MonitorProxyWidget *, Monitor *> m_monitors;

    QTimer *m_refershTimer;
};

} // namespace display

} // namespace dcc

#endif // MONITORSGROUND_H
