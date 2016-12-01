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
    void saveChanges();
    void discardChanges();
    void setPrimary(const int index);
    void setMonitorRotate(Monitor *mon, const quint16 rotate);
    void setMonitorResolution(Monitor *mon, const int mode);
    void setMonitorBrightness(Monitor *mon, const double brightness);

private slots:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);

private:
//    void loadRotations(Monitor * const mon);
//    void loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
//    void loadModes(Monitor * const mon);
//    void loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
    void monitorAdded(const QString &path);
    void updateMonitorBrightness(const QString &monName, const double brightness);

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
};

#endif // DISPLAYWORKER_H
