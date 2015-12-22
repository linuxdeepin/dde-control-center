#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include <QObject>
#include <QtPlugin>

#include "interfaces.h"
#include "controlcenterproxy.h"

class BluetoothMainWidget;
class Bluetooth: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "bluetooth.json")
    Q_INTERFACES(ModuleInterface)

public:
    Bluetooth();
    ~Bluetooth() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

protected:
    void setProxy(ControlCenterProxyInterface *proxy) Q_DECL_OVERRIDE;

private:
    BluetoothMainWidget * m_mainWidget;
};

#endif
