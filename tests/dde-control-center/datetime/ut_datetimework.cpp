#define private public
#include "../src/frame/modules/datetime/datetimework.h"
#undef private


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

    QString timeZone = model->getTimeZone();
    timedate->TimezoneChanged(timeZone.append("1"));
    EXPECT_EQ(model->getTimeZone(), timeZone);

    QString ntpAddress = model->ntpServerAddress();
    timedate->NTPServerChanged(ntpAddress.append("1"));
    EXPECT_EQ(model->ntpServerAddress(), ntpAddress);


    model->userTimeZones();
}

TEST_F(Tst_DateTimeWork, TestDateTimeFormat)
{
    bool format = model->get24HourFormat();
    timedate->Use24HourFormatChanged(!format);
    EXPECT_EQ(model->get24HourFormat(), !format);

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
}

TEST_F(Tst_DateTimeWork, TestWorkSet)
{
#define DCC_DISABLE_TIMEZONE

    EXPECT_NO_THROW(work->setNTP(true));
    EXPECT_NO_THROW(work->set24HourType(true));
    EXPECT_NO_THROW(work->setTimezone("aaaa"));
    EXPECT_NO_THROW(work->addUserTimeZone("aaaa"));
    EXPECT_NO_THROW(work->setNtpServer("1"));
    EXPECT_NO_THROW(work->setWeekdayFormat(1));
    EXPECT_NO_THROW(work->setShortDateFormat(1));
    EXPECT_NO_THROW(work->setLongDateFormat(1));
    EXPECT_NO_THROW(work->setLongTimeFormat(1));
    EXPECT_NO_THROW(work->setShortTimeFormat(1));
    EXPECT_NO_THROW(work->setWeekStartDayFormat(1));

    ZoneInfo info;
    QString str;
    EXPECT_NO_THROW(work->removeUserTimeZone(info));
    EXPECT_NO_THROW(work->GetZoneInfo(str));
    EXPECT_NO_THROW(work->refreshNtpServerList());
    EXPECT_NO_THROW(work->setDatetime(QDateTime::currentDateTime()));
}

TEST_F(Tst_DateTimeWork, TestModelSet)
{
    EXPECT_NO_THROW(model->setWeekdayFormatType(1));
    EXPECT_NO_THROW(model->setShortDateFormat(1));
    EXPECT_NO_THROW(model->setLongDateFormat(1));
    EXPECT_NO_THROW(model->setLongTimeFormat(1));
    EXPECT_NO_THROW(model->setShorTimeFormat(1));
    EXPECT_NO_THROW(model->setWeekStartDayFormat(1));
    EXPECT_NO_THROW(model->setWeekdayFormatTypeCount(1));
    EXPECT_NO_THROW(model->setShortDateFormatTypeCount(1));
    EXPECT_NO_THROW(model->setLongDateFormatTypeCount(1));
    EXPECT_NO_THROW(model->setLongTimeFormatTypeCount(1));
    EXPECT_NO_THROW(model->setShorTimeFormatTypeCount(1));
    EXPECT_NO_THROW(model->setWeekStartDayFormatTypeCount(1));
    
    QStringList lstNtpServer;
    lstNtpServer.append("11");
    EXPECT_NO_THROW(model->setNTPServerList(lstNtpServer));

    const ZoneInfo info;
    EXPECT_NO_THROW(model->addUserTimeZone(info));
    EXPECT_NO_THROW(model->removeUserTimeZone(info));
}