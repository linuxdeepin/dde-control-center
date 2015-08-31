#ifndef SINGLEMONITORSETTINGS_H
#define SINGLEMONITORSETTINGS_H

#include <QFrame>
#include <QMap>

#include <libdui/dbuttongrid.h>
#include <libdui/dslider.h>

#include "dbus/monitorinterface.h"
#include "dbus/displayinterface.h"

DUI_USE_NAMESPACE

class Monitor;
class MonitorGround;
class SingleMonitorSettings : public QFrame
{
    Q_OBJECT
public:
    explicit SingleMonitorSettings(DisplayInterface * display,
                                   const QList<MonitorInterface*> &list,
                                   QWidget *parent = 0);

private:
    QList<MonitorInterface*> m_dbusMonitors;
    DisplayInterface * m_dbusDisplay;
    QMap<ushort, QString> m_rotationMap;
    QList<Monitor*> m_monitors;

    void initUI();
    void initBackend();
    QStringList getResolutionLabels(MonitorInterface *dbus);
    QStringList getRotationLabels(MonitorInterface *dbus);

private slots:
    void updateResolutionButtons(MonitorInterface *dbus, DButtonGrid *resolutionButtons);
    void updateRotationButtons(MonitorInterface *dbus, DButtonGrid *rotationButtons);
    void updateBrightnessSlider(MonitorInterface *dbus, DSlider *brightnessSlider);
};

#endif // SINGLEMONITORSETTINGS_H
