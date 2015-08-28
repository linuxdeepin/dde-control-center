#include <QDebug>

#include "constants.h"

#include "monitorground.h"
#include "monitor.h"

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

    relayout();
}

void MonitorGround::removeMonitor(Monitor *monitor)
{
    m_monitors.removeOne(monitor);

    monitor->setParent(NULL);

    disconnect(monitor, &Monitor::mousePressed, monitor, &Monitor::raise);
    disconnect(monitor, &Monitor::mouseMoveing, this, &MonitorGround::onMonitorMouseMove);
    disconnect(monitor, &Monitor::mouseRelease, this, &MonitorGround::onMonitorMouseRelease);

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
    int destHeight = 140;

    if(m_monitors.count() == 1){
        Monitor *monitor = m_monitors[0];
        QSize resolution = monitor->resolution();
        int destWidth = destHeight * resolution.width() / resolution.height();
        monitor->setFixedSize(destWidth, destHeight);
        QPoint delta = this->rect().center() - monitor->rect().center();
        monitor->move(delta.x(), delta.y());
    }else{
        Monitor *monitor1 = m_monitors[0];
        Monitor *monitor2 = m_monitors[1];

        QSize resolution = monitor1->resolution();
        resolution.setWidth(destHeight / resolution.height() * resolution.width());
        resolution.setHeight(destHeight);
        monitor1->setFixedSize(resolution);

        resolution = monitor2->resolution();
        resolution.setWidth(destHeight / resolution.height() * resolution.width());
        resolution.setHeight(destHeight);
        monitor2->setFixedSize(resolution);

        monitor1->move((height() - monitor1->height()) / 2,
                       (width() - monitor1->width() - monitor2->width()) / 2);
        monitor2->move(monitor1->x() + monitor1->width(), monitor1->y());
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
