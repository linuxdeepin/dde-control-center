#include "../src/frame/modules/datetime/datetimework.h"
#include "../src/frame/modules/datetime/datetimemodel.h"

#include <types/zoneinfo.h>

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dcc::datetime;

class Tst_DateTimeWork : public testing::Test
{
public:
    void SetUp() override
    {
        work = &DatetimeWork::getInstance();
        model = work->model();
        timedate = work->getTimedate();
        work->activate();
    }

    void TearDown() override
    {

    }

public:
    DatetimeWork *work = nullptr;
    DatetimeModel *model = nullptr;
    Timedate *timedate = nullptr;
};

TEST_F(Tst_DateTimeWork, FullTest)
{
    timedate->UserTimezonesChanged(QStringList() << "Asia/Shanghai");

    timedate->TimezoneChanged("aaaa");
    EXPECT_EQ(model->systemTimeZoneId(), "aaaa");

    bool ntp = model->nTP();
    timedate->NTPChanged(!ntp);
    EXPECT_EQ(model->nTP(), !ntp);

    bool format = model->get24HourFormat();
    timedate->Use24HourFormatChanged(!format);
    EXPECT_EQ(model->get24HourFormat(), !format);

    ZoneInfo info = model->currentTimeZone();
    timedate->TimezoneChanged("Asia/Shanghai1");
    EXPECT_EQ(info.getZoneCity(), "上海");

    QString ntpAddress = model->ntpServerAddress();
    timedate->NTPServerChanged(ntpAddress.append("1"));
    EXPECT_EQ(model->ntpServerAddress(), ntpAddress);

    int weektype = model->weekdayFormatType();
    timedate->WeekdayFormatChanged(++weektype);
    EXPECT_EQ(model->weekdayFormatType(), weektype);

    int longformat = model->longDateFormat();
    timedate->LongDateFormatChanged(++longformat);
    EXPECT_EQ(model->longDateFormat(), longformat);

    int shortformat = model->shortDateFormat();
    timedate->ShortDateFormatChanged(++shortformat);
    EXPECT_EQ(model->shortDateFormat(), shortformat);

    int longtime = model->longTimeFormat();
    timedate->LongTimeFormatChanged(++longtime);
    EXPECT_EQ(model->longTimeFormat(), longtime);

    int shorttime = model->shorTimeFormat();
    timedate->ShortTimeFormatChanged(++shorttime);
    EXPECT_EQ(model->shorTimeFormat(), shorttime);

    int weekbegin = model->weekStartDayFormat();
    timedate->WeekBeginsChanged(++weekbegin);
    EXPECT_EQ(model->weekStartDayFormat(), weekbegin);

    work->setNTP(ntp);
    work->setDatetime(QDateTime::currentDateTime());
    work->set24HourType(true);
    work->setTimezone("aaaa");
    work->removeUserTimeZone(info);
    work->addUserTimeZone("aaaa");
    work->setNtpServer("1");
    work->setWeekdayFormat(1);
    work->setShortDateFormat(1);
    work->setLongDateFormat(1);
    work->setLongTimeFormat(1);
    work->setShortTimeFormat(1);
    work->setWeekStartDayFormat(1);

    model->userTimeZones();
}
