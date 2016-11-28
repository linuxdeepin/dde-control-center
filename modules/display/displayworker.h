#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include <QObject>

#include <com_deepin_daemon_display.h>
#include <com_deepin_daemon_display_monitor.h>

using DisplayInter = com::deepin::daemon::Display;
using MonitorInter = com::deepin::daemon::display::Monitor;

class DisplayModel;
class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(DisplayModel *model, QObject *parent = 0);

public slots:
    void rotate();

private slots:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);

private:
    void showRotateDialog(MonitorInter * const inter);

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
};

#endif // DISPLAYWORKER_H
