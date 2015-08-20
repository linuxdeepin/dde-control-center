#ifndef MONITORGROUND_H
#define MONITORGROUND_H

#include <QFrame>

class Monitor;
class MonitorGround : public QFrame
{
    Q_OBJECT
public:
    explicit MonitorGround(QWidget *parent = 0);

    void addMonitor(Monitor * monitor);
    void relayout();

private:
    Monitor * m_monitor;
};

#endif // MONITORGROUND_H
