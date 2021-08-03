#include "../src/frame/window/modules/mouse/touchpadsettingwidget.h"
#include "../src/frame/modules/mouse/mousemodel.h"
#include "../src/frame/modules/mouse/widget/palmdetectsetting.h"
#include "widgets/titledslideritem.h"
#include "widgets/switchwidget.h"
#include "widgets/dccslider.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

using namespace dccV20::mouse;
using namespace dcc::mouse;
using namespace dcc::widgets;

class Tst_TouchPadSettingWidget : public testing::Test
{
    void SetUp() override
    {
        widget = new TouchPadSettingWidget;
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
    TouchPadSettingWidget *widget = nullptr;
    MouseModel *model = nullptr;
};

TEST_F(Tst_TouchPadSettingWidget, emitSignal)
{
    widget->setModel(model);

    QSignalSpy spy1(widget, SIGNAL(requestSetTouchpadMotionAcceleration(const int)));
    TitledSliderItem *item = widget->findChild<TitledSliderItem *>();
    item->slider()->valueChanged(1);
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(widget, SIGNAL(requestSetTapClick(const bool)));
    SwitchWidget *touchClicked = widget->findChild<SwitchWidget *>("touchClicked");
    touchClicked->checkedChanged(!touchClicked->checked());
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(widget, SIGNAL(requestSetTouchNaturalScroll(const bool)));
    SwitchWidget *touchScroll = widget->findChild<SwitchWidget *>("touchNaturalScroll");
    touchScroll->checkedChanged(!touchScroll->checked());
    EXPECT_EQ(spy3.count(), 1);

    QSignalSpy spy4(widget, SIGNAL(requestDetectState(bool)));
    QSignalSpy spy5(widget, SIGNAL(requestContact(int)));
    QSignalSpy spy6(widget, SIGNAL(requestPressure(int)));

    PalmDetectSetting *palm = widget->findChild<PalmDetectSetting *>();
    SwitchWidget *detect = palm->findChild<SwitchWidget *>();
    detect->checkedChanged(!detect->checked());
    EXPECT_EQ(spy4.count(), 1);

    TitledSliderItem *contact = palm->findChild<TitledSliderItem *>("contact");
    contact->slider()->valueChanged(2);
    EXPECT_EQ(spy5.count(), 1);

    TitledSliderItem *pressure = palm->findChild<TitledSliderItem *>("pressure");
    pressure->slider()->valueChanged(2);
    EXPECT_EQ(spy6.count(), 1);

    model->tpadMoveSpeedChanged(1);
}
