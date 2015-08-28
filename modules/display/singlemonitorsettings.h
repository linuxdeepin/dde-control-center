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
    explicit SingleMonitorSettings(DisplayInterface * display, QWidget *parent = 0);

private:
    QList<MonitorInterface*> m_dbusMonitors;
    DisplayInterface * m_dbusDisplay;

    QMap<ushort, QString> m_rotationMap;
    MonitorModeList m_monitorModeList;
    UshortList m_monitorRotations;

    QList<Monitor*> m_monitors;
    MonitorGround * m_monitorGround;
    DButtonGrid * m_resolutionButtons;
    DButtonGrid * m_rotationButtons;
    DSlider * m_brightnessSlider;

    void initUI();
    void initBackend();
    QStringList getResolutionLabels();
    QStringList getRotationLabels();

private slots:
    void updateMonitor();
    void updateResolutionButtons();
    void updateRotationButtons();
    void updateBrightnessSlider();
};

#endif // SINGLEMONITORSETTINGS_H
