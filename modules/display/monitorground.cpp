#include <QDebug>

#include "constants.h"

#include "monitorground.h"
#include "monitor.h"
#include "dbus/monitorinterface.h"

MonitorGround::MonitorGround(QWidget *parent)
    : QFrame(parent)
{
    setStyleSheet(QString("QFrame { background-color: %1 }").arg(DCC::BgDarkColor.name()));

    setFixedSize(310, 210);
}

void MonitorGround::addMonitor(Monitor *monitor)
{
    if(m_monitors.contains(monitor)||!monitor)
        return;

    m_monitors << monitor;
    monitor->setParent(this);

    if(m_monitors.count() == 2){
        m_monitors[0]->setBrother(m_monitors[1]);
        m_monitors[1]->setBrother(m_monitors[0]);

        m_monitors[0]->setDraggable(true);
        m_monitors[1]->setDraggable(true);
    }

    connect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise);
    connect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove);
    connect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease);
    connect(monitor->dbusInterface(), &MonitorInterface::XChanged, this, &MonitorGround::relayout);
    connect(monitor->dbusInterface(), &MonitorInterface::YChanged, this, &MonitorGround::relayout);
    connect(monitor->dbusInterface(), &MonitorInterface::WidthChanged, this, &MonitorGround::relayout);
    connect(monitor->dbusInterface(), &MonitorInterface::HeightChanged, this, &MonitorGround::relayout);
    connect(monitor->dbusInterface(), &MonitorInterface::RotationChanged, this, &MonitorGround::relayout);

    relayout();
}

void MonitorGround::removeMonitor(Monitor *monitor)
{
    m_monitors.removeOne(monitor);

    monitor->setParent(NULL);

    disconnect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise);
    disconnect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove);
    disconnect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease);
    disconnect(monitor->dbusInterface(), &MonitorInterface::XChanged, this, &MonitorGround::relayout);
    disconnect(monitor->dbusInterface(), &MonitorInterface::YChanged, this, &MonitorGround::relayout);
    disconnect(monitor->dbusInterface(), &MonitorInterface::WidthChanged, this, &MonitorGround::relayout);
    disconnect(monitor->dbusInterface(), &MonitorInterface::HeightChanged, this, &MonitorGround::relayout);
    disconnect(monitor->dbusInterface(), &MonitorInterface::RotationChanged, this, &MonitorGround::relayout);

    relayout();
}

void MonitorGround::clear()
{
    foreach (Monitor *monitor, m_monitors) {
        removeMonitor(monitor);
    }
}

void MonitorGround::relayout()
{
    if(m_monitors.count() == 1){
        Monitor *monitor = m_monitors[0];
        MonitorInterface *dbus = monitor->dbusInterface();

        double maxWidth = dbus->x()+dbus->width();
        double maxHeight = dbus->y()+dbus->height();
        double scaleFactor = 0.8 * qMin(width() / maxWidth, height() / maxHeight);

        int x = (width() - maxWidth*scaleFactor) / 2.0;
        int y = (height() - maxHeight*scaleFactor) / 2.0;

        monitor->setFixedSize(scaleFactor*dbus->width(), scaleFactor*dbus->height());
        monitor->move(x + dbus->x()*scaleFactor, y + dbus->y()*scaleFactor);
        monitor->show();
    }else if(m_monitors.count() > 1){
        Monitor *monitor1 = m_monitors[0];
        Monitor *monitor2 = m_monitors[1];
        MonitorInterface *dbus1 = monitor1->dbusInterface();
        MonitorInterface *dbus2 = monitor2->dbusInterface();

        double maxWidth = qMax(dbus1->x()+dbus1->width(), dbus2->x()+dbus2->width());
        double maxHeight = qMax(dbus1->y()+dbus1->height(), dbus2->y()+dbus2->height());
        double scaleFactor = 0.8 * qMin(width() / maxWidth, height() / maxHeight);

        int x = (width() - maxWidth*scaleFactor) / 2.0;
        int y = (height() - maxHeight*scaleFactor) / 2.0;

        monitor1->setFixedSize(scaleFactor*dbus1->width(), scaleFactor*dbus1->height());
        monitor2->setFixedSize(scaleFactor*dbus2->width(), scaleFactor*dbus2->height());
        monitor1->move(x + dbus1->x()*scaleFactor, y + dbus1->y()*scaleFactor);
        monitor2->move(x + dbus2->x()*scaleFactor, y + dbus2->y()*scaleFactor);

        monitor1->show();
        monitor2->show();
    }
}

void MonitorGround::onMonitorMouseMove()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    Monitor *brother = monitor->brother();

    if(brother){
        if(brother->geometry().contains(monitor->geometry().center())){
            brother->dragEnter(monitor);
        }else{
            brother->dragLeave();
        }
    }
}

void MonitorGround::onMonitorMouseRelease()
{
    Monitor *monitor = qobject_cast<Monitor*>(sender());

    if(!monitor)
        return;

    Monitor *brother = monitor->brother();

    if(brother && brother->geometry().contains(monitor->geometry().center())){
        if(brother->drop(monitor)){
            m_monitors.removeOne(monitor);
            brother->setDraggable(false);
            relayout();
        }
    }
}
