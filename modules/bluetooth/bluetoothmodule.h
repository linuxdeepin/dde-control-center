#ifndef BLUETOOTHMODULE_H
#define BLUETOOTHMODULE_H

#include "moduleinterface.h"

namespace dcc {
namespace bluetooth {

class BluetoothWidget;
class BluetoothModel;
class BluetoothWorker;
class Device;
class Adapter;

class BluetoothModule : public QObject, public ModuleInterface
{
    Q_OBJECT
public:
    explicit BluetoothModule(FrameProxyInterface *frame, QObject *parent = 0);

public slots:
    void showDetail(const Adapter *adapter, const Device *device);
    void showBluetoothDetail(const Adapter *adapter);

private:
    ~BluetoothModule();
    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(ContentWidget * const w);
    const QString name() const;
    ModuleWidget *moduleWidget();
    void showPage(const QString &pageName);

private:
    BluetoothWidget *m_bluetoothView;
    BluetoothModel *m_bluetoothModel;
    BluetoothWorker *m_bluetoothWorker;
};

}
}

#endif // BLUETOOTHMODULE_H
