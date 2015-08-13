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
    monitor->setFixedSize(240, 140);

    qDebug() << this->rect() << monitor->rect();

    QPoint delta = this->rect().center() - monitor->rect().center();

    monitor->setParent(this);
    monitor->move(delta.x(), delta.y());
}
