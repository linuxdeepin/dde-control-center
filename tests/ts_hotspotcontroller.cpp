#include "networkcontroller.h"
#include "hotspotcontroller.h"
#include "wirelessdevice.h"

#include <gtest/gtest.h>

using namespace dde::network;

class Tst_HotspotController : public testing::Test
{
public:
    void SetUp() override
    {
        m_controller = NetworkController::instance()->hotspotController();
    }

    void TearDown() override
    {
        m_controller = nullptr;
    }

public:
    HotspotController *m_controller;
};

TEST_F(Tst_HotspotController, controll_test)
{
    qInfo() << m_controller->supportHotspot();

    HotspotController *controller = m_controller;
    auto onDevicesChanged = [ = ] (const QList<WirelessDevice *> &changeDevices) {
        Q_UNUSED(changeDevices);

        QList<WirelessDevice *> devices = controller->devices();
        if (devices.size() > 0) {
            WirelessDevice *device = devices[0];
            QList<HotspotItem *> items = controller->items(device);
            for (HotspotItem *item : items)
                qInfo() << item->connection()->id();

            if (items.size() > 0) {
                HotspotItem *item = items[0];
                controller->connectItem(item);
            }

            device = devices[devices.size() - 1];
            controller->setEnabled(device, false);
        }
    };

    QObject::connect(m_controller, &HotspotController::deviceAdded, onDevicesChanged);
    QObject::connect(m_controller, &HotspotController::deviceRemove, onDevicesChanged);
}
