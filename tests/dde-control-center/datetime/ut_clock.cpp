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
    clock.setAutoNightMode(clock.autoNightMode());
    clock.setDrawTicks(clock.drawTicks());

    ZoneInfo zone;
//    zone.m_zoneName = "Asia/Shanghai";
//    zone.m_zoneCity = "上海";
//    zone.m_utcOffset = 28800;
//    zone.i2 = 0;
//    zone.i3 = 0;
//    zone.i4 = 0;
    clock.setTimeZone(zone);
    clock.update();
}
