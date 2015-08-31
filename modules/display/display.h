#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QtPlugin>

#include <libdui/darrowlineexpand.h>
#include <libdui/libdui_global.h>

#include "interfaces.h"

#include "dbus/displayinterface.h"

DUI_USE_NAMESPACE

class QFrame;
class MonitorGround;
class MonitorInterface;
class Monitor;
class Display: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "display.json")
    Q_INTERFACES(ModuleInterface)

public:
    Display();
    ~Display() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private slots:
    void init();
    void updateUI();

private:
    QFrame * m_frame;
    QVBoxLayout *m_mainLayout;
    DisplayInterface * m_dbusDisplay;
    MonitorGround *m_monitorGround;
    QList<MonitorInterface*> m_dbusMonitors;
    QList<Monitor*> m_monitors;
};

#endif // DISPLAY_H

