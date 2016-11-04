#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include "moduleinterface.h"

class BluetoothModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit BluetoothModule(FrameProxyInterface *frame, QObject *parent = 0);

private:
    ~BluetoothModule();
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();

private:
    ModuleWidget *m_bluetoothView;
};

#endif // BLUETOOTHMODULE_H
