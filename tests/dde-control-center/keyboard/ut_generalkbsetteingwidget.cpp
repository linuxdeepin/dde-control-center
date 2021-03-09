#include "generalkbsettingwidget.h"
#include "keyboardmodel.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"
#include "widgets/switchwidget.h"

#include <QSignalSpy>
#include <QDebug>

#include "gtest/gtest.h"

#include <iostream>

using namespace dccV20::keyboard;
using namespace dcc::keyboard;
using namespace dcc::widgets;

DWIDGET_USE_NAMESPACE

class Tst_GeneralKbSetteingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    GeneralKBSettingWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_GeneralKbSetteingWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new GeneralKBSettingWidget(model);
}

void Tst_GeneralKbSetteingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_GeneralKbSetteingWidget, emitSignal)
{
    TitledSliderItem *repeatDealy = widget->findChild<TitledSliderItem * >("RepeatDelay");
    ASSERT_TRUE(repeatDealy != nullptr);
    QSignalSpy spy1(widget, SIGNAL(requestKBDelayChanged(const int)));
    repeatDealy->slider()->valueChanged(3);
    EXPECT_EQ(spy1.count(), 1);

    TitledSliderItem *repeatRate = widget->findChild<TitledSliderItem * >("RepeatRate");
    ASSERT_TRUE(repeatRate != nullptr);
    QSignalSpy spy2(widget, SIGNAL(requestKBSpeedChanged(const int)));
    repeatRate->slider()->valueChanged(3);
    EXPECT_EQ(spy2.count(), 1);

    SwitchWidget *numLock = widget->findChild<SwitchWidget *>("NumLock");
    ASSERT_TRUE(numLock != nullptr);
    QSignalSpy spyNumLock(widget, SIGNAL(requestNumLockChanged(const bool)));
    numLock->checkedChanged(numLock->checked());
    EXPECT_EQ(spyNumLock.count(), 1);

    SwitchWidget *upper = widget->findChild<SwitchWidget *>("Upper");
    ASSERT_TRUE(upper != nullptr);
    QSignalSpy spyUpper(widget, SIGNAL(requestCapsLockChanged(const bool)));
    upper->checkedChanged(upper->checked());
    EXPECT_EQ(spyUpper.count(), 1);
}
