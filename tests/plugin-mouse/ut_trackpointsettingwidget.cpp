#include "interface/namespace.h"
#include "src/plugin-mouse/window/trackpointsettingwidget.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

DCC_USE_NAMESPACE
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
