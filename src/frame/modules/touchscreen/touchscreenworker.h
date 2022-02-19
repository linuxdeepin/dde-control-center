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

#ifndef TOUCHSCREENWORKER_H
#define TOUCHSCREENWORKER_H

#include <com_deepin_daemon_display.h>

#include <QObject>

using DisplayInter = com::deepin::daemon::Display;

namespace dcc {

namespace display {

class Monitor;
class TouchscreenModel;
class TouchscreenWorker : public QObject
{
    Q_OBJECT

public:
    explicit TouchscreenWorker(TouchscreenModel *model, QObject *parent = 0, bool isSync = false);
    ~TouchscreenWorker();

    void active();

public Q_SLOTS:
    void setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID);

private Q_SLOTS:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);

private:
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);

private:
    TouchscreenModel *m_model;
    DisplayInter m_displayInter;
    QList<Monitor *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // TOUCHSCREENWORKER_H
