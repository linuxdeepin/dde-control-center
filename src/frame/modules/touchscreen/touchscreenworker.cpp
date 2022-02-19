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

#include "touchscreenworker.h"
#include "touchscreenmodel.h"
#include "modules/display/monitor.h"

using namespace dcc::display;

const QString DisplayInterface("com.deepin.daemon.Display");

TouchscreenWorker::TouchscreenWorker(TouchscreenModel *model, QObject *parent, bool isSync)
    : QObject(parent)
    , m_model(model)
    , m_displayInter(DisplayInterface, "/com/deepin/daemon/Display", QDBusConnection::sessionBus(), this)
{
    m_displayInter.setSync(isSync);

    connect(&m_displayInter, &DisplayInter::MonitorsChanged, this, &TouchscreenWorker::onMonitorListChanged);
    connect(&m_displayInter, &DisplayInter::TouchscreensV2Changed, model, &TouchscreenModel::setTouchscreenList);
    connect(&m_displayInter, &DisplayInter::TouchMapChanged, model, &TouchscreenModel::setTouchMap);
    connect(&m_displayInter, &DisplayInter::DisplayModeChanged, model, &TouchscreenModel::setDisplayMode);
}

TouchscreenWorker::~TouchscreenWorker()
{

}

void TouchscreenWorker::active()
{
    onMonitorListChanged(m_displayInter.monitors());
    m_model->setDisplayMode(m_displayInter.displayMode());
    m_model->setTouchscreenList(m_displayInter.touchscreensV2());
    m_model->setTouchMap(m_displayInter.touchMap());
}

void TouchscreenWorker::setTouchScreenAssociation(const QString &monitor, const QString &touchscreenUUID)
{
    m_displayInter.AssociateTouchByUUID(monitor, touchscreenUUID);
}

void TouchscreenWorker::onMonitorListChanged(const QList<QDBusObjectPath> &mons)
{
    QList<QString> ops;
    for (const auto *mon : m_monitors)
        ops << mon->path();

    QList<QString> pathList;
    for (const auto &op : mons) {
        const QString path = op.path();
        pathList << path;
        if (!ops.contains(path))
            monitorAdded(path);
    }

    for (const auto &op : ops)
        if (!pathList.contains(op))
            monitorRemoved(op);
}

void TouchscreenWorker::monitorAdded(const QString &path)
{
    MonitorInter *inter = new MonitorInter(DisplayInterface, path, QDBusConnection::sessionBus(), this);
    Monitor *mon = new Monitor(this);

    connect(inter, &MonitorInter::XChanged, mon, &Monitor::setX);
    connect(inter, &MonitorInter::YChanged, mon, &Monitor::setY);
    connect(inter, &MonitorInter::WidthChanged, mon, &Monitor::setW);
    connect(inter, &MonitorInter::HeightChanged, mon, &Monitor::setH);
    connect(inter, &MonitorInter::NameChanged, mon, &Monitor::setName);

    mon->setX(inter->x());
    mon->setY(inter->y());
    mon->setW(inter->width());
    mon->setH(inter->height());
    mon->setName(inter->name());
    mon->setPath(path);

    m_model->monitorAdded(mon);
    m_monitors.append(mon);

    inter->setSync(false);
}

void TouchscreenWorker::monitorRemoved(const QString &path)
{
    Monitor *monitor = nullptr;
    for (const auto mon : m_monitors) {
        if (mon->path() == path) {
            monitor = mon;
            break;
        }
    }

    if (!monitor)
        return;

    m_model->monitorRemoved(monitor);
    m_monitors.removeOne(monitor);
    monitor->deleteLater();
}
