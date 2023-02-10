#include <gtest/gtest.h>

#include "interface/namespace.h"
#include "src/plugin-notification/operation/notificationworker.h"
#include "src/plugin-notification/window/notificationwidget.h"

#include <DListView>

#include <QSignalSpy>
#include <QTest>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_NotificationWidget : public testing::Test
{
public:
    void SetUp() override { }

    void TearDown() override { }

public:
};

TEST_F(Tst_NotificationWidget, init)
{
    NotificationModel model;
    NotificationWidget widget(&model, new QStandardItemModel());
}

TEST_F(Tst_NotificationWidget, FullTest)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    NotificationWidget widget(&model, new QStandardItemModel());

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
