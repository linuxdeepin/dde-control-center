#include "../src/frame/window/modules/mouse/trackpointsettingwidget.h"
#include "../src/frame/modules/mouse/mousemodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dccV20::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

class Tst_TrackPointSettingWidget : public testing::Test
{
    void SetUp() override
    {
        widget = new TrackPointSettingWidget;
        model = new MouseModel;
    }

    void TearDown() override
    {
        delete widget;
        widget = nullptr;
        delete model;
        model = nullptr;
    }

public:
    TrackPointSettingWidget *widget = nullptr;
    MouseModel *model = nullptr;
};

TEST_F(Tst_TrackPointSettingWidget, emitSignal)
{
    widget->setModel(model);

    QSignalSpy spy1(widget, SIGNAL(requestSetTrackPointMotionAcceleration(const int)));
    TitledSliderItem *item = widget->findChild<TitledSliderItem *>();
    item->slider()->valueChanged(1);
    EXPECT_EQ(spy1.count(), 1);
}
