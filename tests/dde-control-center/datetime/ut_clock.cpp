#include "../src/frame/modules/datetime/clock.h"

#include <gtest/gtest.h>

using namespace dcc::datetime;

class Tst_Clock : public testing::Test
{
public:
    void SetUp() override
    {

    }

    void TearDown() override
    {

    }

public:
};

TEST_F(Tst_Clock, FullTest)
{
    Clock clock;

    clock.setAutoNightMode(true);
    EXPECT_EQ(clock.autoNightMode(), true);
    clock.setDrawTicks(true);
    EXPECT_EQ(clock.drawTicks(), true);

    ZoneInfo zone;
    clock.setTimeZone(zone);
    clock.update();
}
