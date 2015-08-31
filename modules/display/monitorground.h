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
    void removeMonitor(Monitor * monitor);
    void clear();

private slots:
    void relayout();
    void onMonitorMouseMove();
    void onMonitorMouseRelease();

private:
    QList<Monitor*> m_monitors;
};

#endif // MONITORGROUND_H
