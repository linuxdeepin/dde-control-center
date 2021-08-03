#include "../src/frame/window/modules/bluetooth/detailpage.h"
#include "../src/frame/modules/bluetooth/bluetoothworker.h"
#include "../src/frame/modules/bluetooth/bluetoothmodel.h"

#include <QSignalSpy>
#include <QPushButton>
#include <QLineEdit>
#include <QTest>

#include <gtest/gtest.h>

using namespace dccV20::bluetooth;
using namespace dcc::bluetooth;

class Tst_DetailPage : public testing::Test
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

TEST_F(Tst_DetailPage, getAdapter)
{
    QString str = R"({"Path":"/org/bluez/hci0","Name":"UnitTest","Alias":"UnitTest","Powered":true,"Discovering":true,"Discoverable":true,"DiscoverableTimeout":0})";
    QJsonObject json = QJsonDocument::fromJson(str.toUtf8()).object();
    bluetooth->AdapterAdded(str);

    QString device = R"({"Path":"/org/bluez/hci0/dev_A4_50_46_BC_4A_5B","AdapterPath":"/org/bluez/hci0","Alias":"UnitTest","Trusted":true,"Paired":false,"State":0,"ServicesResolved":false,"ConnectState":false,"UUIDs":[],"Name":"UnitTest","Icon":"phone","RSSI":-73,"Address":"A4:50:46:BC:4A:5B"})";
    bluetooth->DeviceAdded(device);

    EXPECT_GT(model->adapters().count(), 0);
    EXPECT_GT(model->adapterById("/org/bluez/hci0")->devices().count(), 0);
}

TEST_F(Tst_DetailPage, FullTest)
{
    adapter = model->adapterById("/org/bluez/hci0");
    const Device *device = model->adapterById("/org/bluez/hci0")->deviceById("/org/bluez/hci0/dev_A4_50_46_BC_4A_5B");
    DetailPage page(model, adapter, device);

    qRegisterMetaType<const dcc::bluetooth::Adapter *>("const dcc::bluetooth::Adapter *");
    qRegisterMetaType<const dcc::bluetooth::Device *>("const dcc::bluetooth::Device *");

    QPushButton *ignore = page.findChild<QPushButton *>("ignoreButton");
    QSignalSpy spy1(&page, SIGNAL(requestIgnoreDevice(const dcc::bluetooth::Adapter *, const dcc::bluetooth::Device *)));
    QTest::mouseClick(ignore, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(0, 0));
    EXPECT_EQ(spy1.count(), 1);

    QPushButton *disconnect = page.findChild<QPushButton *>("disconnectButton");
    QSignalSpy spy2(&page, SIGNAL(requestDisconnectDevice(const dcc::bluetooth::Device *)));
    QTest::mouseClick(disconnect, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(0, 0));
    EXPECT_EQ(spy2.count(), 1);

    QPushButton *connect = page.findChild<QPushButton *>("connectButton");
    QSignalSpy spy3(&page, SIGNAL(requestConnectDevice(const dcc::bluetooth::Device *, const dcc::bluetooth::Adapter *)));
    QTest::mouseClick(connect, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(0, 0));
    EXPECT_EQ(spy3.count(), 1);

    QLineEdit *edit = page.findChild<QLineEdit *>();
    QSignalSpy spy4(&page, SIGNAL(requestSetDevAlias(const dcc::bluetooth::Device *, const QString &)));
    edit->setText("123456789123456789123456789123456789");
    edit->setText("bbb");
    edit->editingFinished();
    edit->setText(QString());
    edit->editingFinished();
    EXPECT_EQ(spy4.count(), 2);

    Device *temp = const_cast<Device *>(device);
    temp->setState(Device::State::StateConnected, false);
    temp->setState(Device::State::StateConnected, true);
    temp->setState(Device::State::StateAvailable, true);
    temp->setState(Device::State::StateDisconnecting, true);
    temp->setState(Device::State::StateUnavailable, true);
    temp->setPaired(!temp->paired());
}
