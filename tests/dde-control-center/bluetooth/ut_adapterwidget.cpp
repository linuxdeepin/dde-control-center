#include "../src/frame/window/modules/bluetooth/bluetoothwidget.h"
#include "../src/frame/window/modules/bluetooth/adapterwidget.h"
#include "../src/frame/window/modules/bluetooth/titleedit.h"
#include "../src/frame/modules/bluetooth/bluetoothworker.h"
#include "../src/frame/modules/bluetooth/bluetoothmodel.h"
#include "widgets/switchwidget.h"

#include <DCheckBox>
#include <DIconButton>
#include <DListView>

#include <QSignalSpy>

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dccV20::bluetooth;
using namespace dcc::bluetooth;
using namespace dcc::widgets;

class Tst_AdapterWidget : public testing::Test
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
    const Adapter *adapter;
};

TEST_F(Tst_AdapterWidget, getAdapter)
{
    QString str = R"({"Path":"/org/bluez/hci0","Name":"UnitTest","Alias":"UnitTest","Powered":true,"Discovering":true,"Discoverable":true,"DiscoverableTimeout":0})";
    QJsonObject json = QJsonDocument::fromJson(str.toUtf8()).object();
    bluetooth->AdapterAdded(str);

    QString device = R"({"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":true,"Paired":true,"State":0,"ServicesResolved":false,"ConnectState":false,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DeviceAdded(device);
    device = R"({"Path":"aaa","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":true,"Paired":false,"State":0,"ServicesResolved":false,"ConnectState":false,"UUIDs":[],"Name":"UnitTest","Icon":"","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DeviceAdded(device);

    EXPECT_GT(model->adapters().count(), 0);
    EXPECT_GT(model->adapterById("/org/bluez/hci0")->devices().count(), 0);
}

TEST_F(Tst_AdapterWidget, FullTest)
{
    adapter = model->adapterById("/org/bluez/hci0");
    const Device *device = adapter->deviceById("/org/bluez/hci0/dev_A4_50_46_BC_4A_5B");
    AdapterWidget widget(adapter, model);
    adapter->disconnect();
    widget.setAdapter(adapter);

    widget.getSwitchState();

    qRegisterMetaType<const dcc::bluetooth::Adapter *>("const dcc::bluetooth::Adapter *");
    qRegisterMetaType<const dcc::bluetooth::Device *>("const dcc::bluetooth::Device *");

    SwitchWidget *power = widget.findChild<SwitchWidget *>("powerSwitch");
    QSignalSpy spy1(&widget, SIGNAL(requestSetToggleAdapter(const dcc::bluetooth::Adapter *, const bool &)));
    power->checkedChanged(true);
    power->checkedChanged(false);
    EXPECT_EQ(spy1.count(), 2);

    SwitchWidget *discovery = widget.findChild<SwitchWidget *>("discoverySwitch");
    QSignalSpy spy2(&widget, SIGNAL(requestDiscoverable(const dcc::bluetooth::Adapter *, const bool &)));
    discovery->checkedChanged(true);
    EXPECT_EQ(spy2.count(), 1);

    DCheckBox *checkBox = widget.findChild<DCheckBox *>();
    checkBox->stateChanged(0);
    checkBox->stateChanged(1);
    checkBox->stateChanged(2);

    DIconButton *refresh = widget.findChild<DIconButton *>();
    QSignalSpy spy3(&widget, SIGNAL(requestRefresh(const dcc::bluetooth::Adapter *)));
    refresh->click();
    EXPECT_EQ(spy3.count(), 1);

    DListView *myDevice = widget.findChild<DListView *>("myDeviceListView");
    DListView *otherDevice = widget.findChild<DListView *>("otherDeviceListView");
    int a = 0, b = 0;
    QSignalSpy spy4(&widget, SIGNAL(requestConnectDevice(const dcc::bluetooth::Device *, const dcc::bluetooth::Adapter *)));
    if (myDevice->model()->rowCount()) {
        myDevice->clicked(myDevice->model()->index(0, 0));
        a++;
    }

    if (otherDevice->model()->rowCount()) {
        otherDevice->clicked(otherDevice->model()->index(0, 0));
        b++;
    }
    EXPECT_EQ(spy4.count(), a + b);

    QSignalSpy spy5(&widget, SIGNAL(notifyRemoveDevice()));
    QMetaObject::invokeMethod(&widget, "removeDevice", Qt::DirectConnection, Q_ARG(QString, "/org/bluez/hci0/dev_A4_50_46_BC_4A_5B"));
    EXPECT_EQ(spy5.count(), 1);
    QMetaObject::invokeMethod(&widget, "removeDevice", Qt::DirectConnection, Q_ARG(QString, "aaa"));

    TitleEdit *edit = widget.findChild<TitleEdit *>();
    QSignalSpy spy6(edit, SIGNAL(requestSetBluetoothName(const QString &)));
    edit->setTitle("123456789132456789132456789132456789");
    edit->setName();
    edit->setEdit();
    EXPECT_EQ(spy6.count(), 1);

    Device *temp = const_cast<Device *>(device);
    temp->setPaired(temp->paired());
    device->address();
    device->connecting();
}
