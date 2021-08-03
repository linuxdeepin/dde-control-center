#include <gtest/gtest.h>
#include "../src/frame/modules/bluetooth/bluetoothworker.h"

#include <QSignalSpy>

using namespace dcc::bluetooth;

class Tst_BluetoothWorker : public testing::Test
{
public:
    void SetUp() override
    {
        worker = &BluetoothWorker::Instance(false);
        model = worker->model();
        bluetooth = worker->getDBusObject();
        worker->activate();
    }

    void TearDown() override
    {
        worker->deactivate();
    }

public:
    BluetoothWorker *worker;
    BluetoothModel *model;
    DBusBluetooth *bluetooth;
};

TEST_F(Tst_BluetoothWorker, emitSignal)
{
    QString adapter = R"({"Path":"/org/bluez/hci0","Name":"UnitTest","Alias":"UnitTest","Powered":true,"Discovering":true,"Discoverable":true,"DiscoverableTimeout":0})";
    QJsonObject json = QJsonDocument::fromJson(adapter.toUtf8()).object();
    bluetooth->AdapterAdded(adapter);
    const Adapter *ada = model->adapterById("/org/bluez/hci0");
    EXPECT_EQ(ada->id(), "/org/bluez/hci0");

    adapter = R"({"Path":"/org/bluez/hci0","Name":"UnitTest","Alias":"UnitTest","Powered":false,"Discovering":true,"Discoverable":true,"DiscoverableTimeout":0})";
    bluetooth->AdapterPropertiesChanged(adapter);
    EXPECT_EQ(ada->powered(), false);

    QString device = R"({"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":true,"Paired":false,"State":0,"ServicesResolved":false,"ConnectState":false,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DeviceAdded(device);
    const Device *dev = ada->deviceById("/org/bluez/hci0/dev_A4_50_46_BC_4A_5B");
    EXPECT_EQ(dev->name(), "UnitTest");

    device = R"({"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":false,"Paired":true,"State":0,"ServicesResolved":false,"ConnectState":true,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DevicePropertiesChanged(device);
    EXPECT_EQ(dev->paired(), true);

    QSignalSpy spy1(worker, SIGNAL(pinCodeCancel(const QDBusObjectPath &)));
    bluetooth->Cancelled(QDBusObjectPath());
    EXPECT_EQ(spy1.count(), 1);

    bluetooth->RequestAuthorization(QDBusObjectPath());

    QSignalSpy spy2(worker, SIGNAL(requestConfirmation(const QDBusObjectPath &, const QString &)));
    bluetooth->RequestConfirmation(QDBusObjectPath(), QString());
    EXPECT_EQ(spy2.count(), 1);

    bluetooth->RequestPasskey(QDBusObjectPath());

    bluetooth->RequestPinCode(QDBusObjectPath());

    PinCodeDialog *dlg = PinCodeDialog::instance("0", false);
    QTimer::singleShot(10, [dlg] {
        dlg->close();
    });
    bluetooth->DisplayPasskey(QDBusObjectPath(), 0, 0);

    PinCodeDialog *dlg1 = PinCodeDialog::instance("", false);
    QTimer::singleShot(10, [dlg1] {
        dlg1->close();
    });
    bluetooth->DisplayPinCode(QDBusObjectPath(), QString());

    bool transportable = model->canTransportable();
    bluetooth->TransportableChanged(!transportable);
    EXPECT_EQ(!transportable, model->canTransportable());

    worker->setAdapterPowered(model->adapterById("/org/bluez/hci0"), false);
    worker->setAdapterPowered(model->adapterById("/org/bluez/hci0"), true);
    worker->connectDevice(dev, ada);
    worker->disconnectDevice(dev);
    worker->setAlias(ada, "aaa");
    worker->onRequestSetDiscoverable(ada, false);
    worker->onRequestSetDiscoverable(ada, true);
    worker->setDeviceAlias(dev, "bbb");
    worker->pinCodeConfirm(QDBusObjectPath(), false);
    worker->pinCodeConfirm(QDBusObjectPath(), true);
    worker->ignoreDevice(ada, dev);

    QSignalSpy spy(ada, SIGNAL(deviceRemoved(const QString &)));
    bluetooth->DeviceRemoved(device);
    EXPECT_EQ(spy.count(), 1);
    delete dev;

    int old = model->adapters().count();
    bluetooth->AdapterRemoved(adapter);
    EXPECT_LT(model->adapters().count(), old);
}
