#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include "monitor.h"

#include <QObject>

#include <com_deepin_daemon_display.h>

using DisplayInter = com::deepin::daemon::Display;

class DisplayModel;
class DisplayWorker : public QObject
{
    Q_OBJECT

public:
    explicit DisplayWorker(DisplayModel *model, QObject *parent = 0);
    ~DisplayWorker();

public slots:
    void rotate();

private slots:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void setMonitorRotate(Monitor *mon, const quint16 rotate);

private:
    void showRotateDialog(Monitor * const mon);
    void monitorAdded(const QString &path);

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
};

#endif // DISPLAYWORKER_H
