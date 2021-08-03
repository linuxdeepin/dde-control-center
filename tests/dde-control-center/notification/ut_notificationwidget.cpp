#include "../src/frame/window/modules/notification/notificationwidget.h"
#include "../src/frame/modules/notification/notificationworker.h"
#include "interface/namespace.h"

#include <DListView>

#include <QSignalSpy>
#include <QTest>

#include <gtest/gtest.h>

using namespace DCC_NAMESPACE::notification;
using namespace dcc::notification;
DWIDGET_USE_NAMESPACE

class Tst_NotificationWidget : public testing::Test
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

TEST_F(Tst_NotificationWidget, init)
{
    NotificationModel model;
    NotificationWidget widget(&model);
}

TEST_F(Tst_NotificationWidget, FullTest)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    NotificationWidget widget(&model);

    QSignalSpy spy1(&widget, SIGNAL(requestShowApp(int)));
    DListView *applist = widget.getAppListview();
    applist->clicked(applist->model()->index(applist->model()->rowCount() - 1, 0));
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(&widget, SIGNAL(requestShowSystem()));
    DListView *syslist = widget.getSysListview();
    syslist->clicked(syslist->model()->index(0, 0));
    EXPECT_EQ(spy2.count(), 1);

    widget.setAppCurrentIndex(2);
    widget.setAppCurrentIndex(100);
}
