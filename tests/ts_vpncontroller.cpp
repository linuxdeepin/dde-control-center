#include "networkcontroller.h"
#include "networkdevicebase.h"
#include "networkdetails.h"
#include "vpncontroller.h"

#include <QDebug>

#include <gtest/gtest.h>

using namespace dde::network;

class Tst_VPNController : public testing::Test
{
public:
    void SetUp() override
    {
        m_controller = NetworkController::instance()->vpnController();
    }

    void TearDown() override
    {
        m_controller = nullptr;
    }

public:
    VPNController *m_controller;
};

TEST_F(Tst_VPNController, dslcontroll_test)
{
    VPNController *controller = m_controller;

    auto itemChanged = [ = ](const QList<VPNItem *> &changedItems) {
        Q_UNUSED(changedItems);
        QList<VPNItem *> items = controller->items();
        for (VPNItem *item : items) {
            qInfo() << item->connection()->id()
                       << "," << item->connection()->path()
                       << "," << item->connection()->ssid()
                       << "," << item->connection()->uuid()
                       << "," << item->connection()->hwAddress()
                       << "," << item->connection()->clonedAddress();
        }

        if (items.size() > 0) {
            VPNItem *item = items[0];
            controller->connectItem(item);
        }

        if (controller->enabled())
            controller->setEnabled(false);
    };

    QObject::connect(m_controller, &VPNController::itemAdded, itemChanged);
    QObject::connect(m_controller, &VPNController::itemRemoved, itemChanged);
}
