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
    m_monitor = monitor;
    m_monitor->setParent(this);

    relayout();
}

void MonitorGround::relayout()
{
    int destHeight = 140;

    QSize resolution = m_monitor->resolution();

    int destWidth = destHeight * resolution.width() / resolution.height();

    m_monitor->setFixedSize(destWidth, destHeight);

    QPoint delta = this->rect().center() - m_monitor->rect().center();

    m_monitor->move(delta.x(), delta.y());
}
