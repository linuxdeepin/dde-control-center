#include "networkcontroller.h"
#include "dslcontroller.h"

#include <gtest/gtest.h>

using namespace dde::network;

class Tst_DSLController : public testing::Test
{
public:
    void SetUp() override
    {
        m_controller = NetworkController::instance()->dslController();
    }

    void TearDown() override
    {
        m_controller = nullptr;
    }

public:
    DSLController *m_controller;
};

TEST_F(Tst_DSLController, dslcontroll_test)
{
    DSLController *controller = m_controller;
    auto dslChanged = [ = ](const QList<DSLItem *> &changeItems) {
        Q_UNUSED(changeItems);

        QList<DSLItem *> items = controller->items();
        for (DSLItem *item : items)
            qWarning() << item->connection()->id() << "," << item->connection()->path()
                       << item->connection()->ssid() << "," << item->connection()->uuid();

        if (items.size() > 0) {
            DSLItem *item = items[0];
            controller->connectItem(item);
        }
    };

    QObject::connect(m_controller, &DSLController::itemAdded, dslChanged);
    QObject::connect(m_controller, &DSLController::itemRemoved, dslChanged);
}
