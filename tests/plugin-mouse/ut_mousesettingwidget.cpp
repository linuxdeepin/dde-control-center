#include "interface/namespace.h"
#include "src/plugin-mouse/window/mousesettingwidget.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/switchwidget.h"
#include "widgets/dccslider.h"

#include <QSignalSpy>

#include <gtest/gtest.h>

DCC_USE_NAMESPACE

class Tst_MouseSettingWidget : public testing::Test
{
    void SetUp() override
    {
        widget = new MouseSettingWidget;
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
    MouseSettingWidget *widget = nullptr;
    MouseModel *model = nullptr;
};

TEST_F(Tst_MouseSettingWidget, emitSignal)
{
    widget->setModel(model);

    QSignalSpy spy1(widget, SIGNAL(requestSetMouseMotionAcceleration(const int)));
    TitledSliderItem *mouseMove = widget->findChild<TitledSliderItem *>();
    mouseMove->slider()->valueChanged(1);
    EXPECT_EQ(spy1.count(), 1);

    QSignalSpy spy2(widget, SIGNAL(requestSetAccelProfile(const bool)));
    SwitchWidget *adapt = widget->findChild<SwitchWidget *>("adaptiveAccelProfile");
    adapt->checkedChanged(!adapt->checked());
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(widget, SIGNAL(requestSetDisTouchPad(const bool)));
    SwitchWidget *disable = widget->findChild<SwitchWidget *>("disableTouchPad");
    disable->checkedChanged(!disable->checked());
    EXPECT_EQ(spy3.count(), 1);

    QSignalSpy spy4(widget, SIGNAL(requestSetMouseNaturalScroll(const bool)));
    SwitchWidget *mouse = widget->findChild<SwitchWidget *>("mouseNaturalScroll");
    mouse->checkedChanged(!mouse->checked());
    EXPECT_EQ(spy4.count(), 1);
}
