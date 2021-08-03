#include "../src/frame/window/modules/mouse/mousewidget.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dccV20::mouse;

class Tst_MouseWidget : public testing::Test
{
    void SetUp() override
    {
        widget = new MouseWidget;
    }

    void TearDown() override
    {
        delete widget;
        widget = nullptr;
    }

public:
    MouseWidget *widget = nullptr;
};

TEST_F(Tst_MouseWidget, emitSignal)
{
    widget->init(true, true);

    QSignalSpy spy1(widget, SIGNAL(showGeneralSetting()));
    widget->setDefaultWidget();
    EXPECT_EQ(spy1.count(), 1);

    QListView *list = widget->findChild<QListView *>();
    list->clicked(list->model()->index(1, 0));
    list->clicked(list->model()->index(0, 0));
    EXPECT_EQ(spy1.count(), 2);

    QSignalSpy spy2(widget, SIGNAL(showMouseSetting()));
    list->clicked(list->model()->index(1, 0));
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(widget, SIGNAL(showTouchpadSetting()));
    list->clicked(list->model()->index(2, 0));
    EXPECT_EQ(spy3.count(), 1);

    QSignalSpy spy4(widget, SIGNAL(showTrackPointSetting()));
    list->clicked(list->model()->index(3, 0));
    EXPECT_EQ(spy4.count(), 1);

    widget->initSetting(1);
    EXPECT_EQ(spy2.count(), 2);
}
