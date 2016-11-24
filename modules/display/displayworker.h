#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include <QObject>

#include <com_deepin_daemon_display.h>
#include <com_deepin_daemon_display_monitor.h>

using DisplayInter = com::deepin::daemon::Display;
using MonitorInter = com::deepin::daemon::display::Monitor;

class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(QObject *parent = 0);

private:
    DisplayInter m_displayInter;
    QMap<QString, MonitorInter*> m_monitors;
};

#endif // DISPLAYWORKER_H
