#include "../src/frame/window/modules/bluetooth/bluetoothwidget.h"
#include "../src/frame/window/modules/bluetooth/adapterwidget.h"
#include "../src/frame/modules/bluetooth/bluetoothworker.h"
#include "../src/frame/modules/bluetooth/bluetoothmodel.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dccV20::bluetooth;
using namespace dcc::bluetooth;

class Tst_BluetoothWidget : public testing::Test
{
public:
    void SetUp() override
    {
        worker = &BluetoothWorker::Instance(false);
        model = worker->model();
        bluetooth = worker->getDBusObject();
        widget = new BluetoothWidget(model);
        worker->activate();
    }

    void TearDown() override
    {
        delete widget;
        widget = nullptr;
        worker->deactivate();
    }

public:
    BluetoothWidget *widget;
    BluetoothWorker *worker;
    BluetoothModel *model;
    DBusBluetooth *bluetooth;
    const Adapter *adapter;
};

TEST_F(Tst_BluetoothWidget, getAdapter)
{
    QString str = R"({"Path":"/org/bluez/hci0","Name":"UnitTest","Alias":"UnitTest","Powered":true,"Discovering":true,"Discoverable":true,"DiscoverableTimeout":0})";
    QJsonObject json = QJsonDocument::fromJson(str.toUtf8()).object();
    bluetooth->AdapterAdded(str);

    QString device = R"({"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":true,"Paired":false,"State":0,"ServicesResolved":false,"ConnectState":false,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DeviceAdded(device);

    EXPECT_GT(model->adapters().count(), 0);
    EXPECT_GT(model->adapterById("/org/bluez/hci0")->devices().count(), 0);
}

TEST_F(Tst_BluetoothWidget, FullTest)
{
    widget->setModel(model);
    adapter = model->adapterById("/org/bluez/hci0");
    const Device* device = adapter->deviceById("/org/bluez/hci0/dev_A4_50_46_BC_4A_5B");

    widget->loadDetailPage();

    AdapterWidget *adaWidget = nullptr;
    if (widget->getAdapterMap().contains(adapter)) {
        adaWidget = widget->getAdapterMap().value(adapter);
    }
    ASSERT_NE(adaWidget, nullptr);

    qRegisterMetaType<const dcc::bluetooth::Adapter *>("const dcc::bluetooth::Adapter *");
    qRegisterMetaType<const dcc::bluetooth::Device *>("const dcc::bluetooth::Device *");

    QSignalSpy spy1(widget, SIGNAL(requestSetToggleAdapter(const dcc::bluetooth::Adapter *, const bool &)));
    adaWidget->requestSetToggleAdapter(adapter, false);
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(widget, SIGNAL(requestConnectDevice(const dcc::bluetooth::Device *, const dcc::bluetooth::Adapter *)));
    adaWidget->requestConnectDevice(device, adapter);
    EXPECT_EQ(spy2.count(), 1);


    QSignalSpy spy3(widget, SIGNAL(requestDisconnectDevice(const dcc::bluetooth::Device *)));
    adaWidget->requestDisconnectDevice(device);
    EXPECT_EQ(spy3.count(), 1);

    QSignalSpy spy4(widget, SIGNAL(showDeviceDetail(const dcc::bluetooth::Adapter *, const dcc::bluetooth::Device *)));
    adaWidget->requestShowDetail(adapter, device);
    EXPECT_EQ(spy4.count(), 1);

    QSignalSpy spy5(widget, SIGNAL(requestSetAlias(const dcc::bluetooth::Adapter *, const QString &)));
    adaWidget->requestSetAlias(adapter, "device");
    EXPECT_EQ(spy5.count(), 1);

    QSignalSpy spy6(widget, SIGNAL(requestDiscoverable(const dcc::bluetooth::Adapter *, const bool &)));
    adaWidget->requestDiscoverable(adapter, false);
    EXPECT_EQ(spy6.count(), 1);

    adaWidget->notifyRemoveDevice();
    adaWidget->notifyLoadFinished();
    adaWidget->requestRefresh(adapter);
}
