#include "../src/frame/modules/datetime/timezoneitem.h"
#include "../src/frame/modules/datetime/datetimework.h"

#include <types/zoneinfo.h>
#include <DIconButton>

#include <QSignalSpy>
#include <QTest>

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::datetime;

class Tst_TimezoneItem : public testing::Test
{
public:
    void SetUp() override
    {
        timedate = (&DatetimeWork::getInstance())->getTimedate();
    }

    void TearDown() override
    {

    }

public:
    Timedate *timedate = nullptr;
};

TEST_F(Tst_TimezoneItem, FullTest)
{
    TimezoneItem item;
    ZoneInfo info = timedate->GetZoneInfo("Asia/Shanghai").value();
    item.setTimeZone(info);
    item.toRemoveMode();
    item.toNormalMode();
    item.setDetailVisible(false);
}

TEST_F(Tst_TimezoneItem, buttonClicked)
{
    TimezoneItem item;

    QSignalSpy spy(&item, SIGNAL(removeClicked()));
    DIconButton *button = item.findChild<DIconButton *>("remove_button");
    QTest::mouseClick(button, Qt::MouseButton::LeftButton, Qt::KeyboardModifier::NoModifier, QPoint(0, 0));
    EXPECT_EQ(spy.count(), 1);
}
