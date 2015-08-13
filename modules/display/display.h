#ifndef DISPLAY_H
#define DISPLAY_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

#include "dbus/displayinterface.h"

class QFrame;
class Display: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "display.json")
    Q_INTERFACES(ModuleInterface)

public:
    Display();
    ~Display() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame * m_frame;
    com::deepin::daemon::Display * m_dbusDisplay;
};

#endif // DISPLAY_H

