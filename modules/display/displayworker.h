#ifndef DISPLAYWORKER_H
#define DISPLAYWORKER_H

#include "monitor.h"

#include <QObject>

#include <com_deepin_daemon_display.h>

using DisplayInter = com::deepin::daemon::Display;

#define CUSTOM_MODE     0
#define MERGE_MODE      1
#define EXTEND_MODE     2
#define SINGLE_MODE     3

namespace dcc {

namespace display {

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
    void mergeScreens();
    void splitScreens();
//    void switchCustom(const bool deleteConfig = true);
    void createConfig();
    void switchConfig(const QString &config);
    void deleteConfig(const QString &config);
    void modifyConfigName(const QString &oldName, const QString &newName);
    void switchMode(const int mode, const QString &name = QString());
    void setPrimary(const int index);
    void setMonitorEnable(Monitor *mon, const bool enabled);
#ifndef DCC_DISABLE_ROTATE
    void setMonitorRotateAll(const quint16 rotate);
    void setMonitorRotate(Monitor *mon, const quint16 rotate);
#endif
    void setMonitorResolution(Monitor *mon, const int mode);
    void setMonitorBrightness(Monitor *mon, const double brightness);
    void setMonitorPosition(Monitor *mon, const int x, const int y);

private slots:
    void onMonitorListChanged(const QList<QDBusObjectPath> &mons);
    void onMonitorsBrightnessChanged(const BrightnessMap &brightness);
    void onModifyConfigNameFinished(QDBusPendingCallWatcher *w);

private:
//    void loadRotations(Monitor * const mon);
//    void loadRotationsFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
//    void loadModes(Monitor * const mon);
//    void loadModesFinished(Monitor * const mon, QDBusPendingCallWatcher *watcher);
    void createConfigFinshed(QDBusPendingCallWatcher *w);
    void monitorAdded(const QString &path);
    void monitorRemoved(const QString &path);
    void updateMonitorBrightness(const QString &monName, const double brightness);

private:
    DisplayModel *m_model;
    DisplayInter m_displayInter;
    QMap<Monitor *, MonitorInter *> m_monitors;
};

} // namespace display

} // namespace dcc

#endif // DISPLAYWORKER_H
