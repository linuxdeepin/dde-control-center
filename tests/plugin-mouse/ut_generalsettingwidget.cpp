#include "interface/namespace.h"
#include "src/plugin-mouse/window/generalsettingwidget.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/window/doutestwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/titledslideritem.h"
#include "widgets/dccslider.h"

#include <dpicturesequenceview.h>

#include <QSignalSpy>
#include <QApplication>
#include <QMouseEvent>

#include <gtest/gtest.h>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class Tst_GeneralSettingWidget : public testing::Test
{
    void SetUp() override
    {
        widget = new GeneralSettingWidget;
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
    GeneralSettingWidget *widget = nullptr;
    MouseModel *model = nullptr;
};

TEST_F(Tst_GeneralSettingWidget, emitSignal)
{
    widget->setModel(model);

    SwitchWidget *left = widget->findChild<SwitchWidget *>("leftHand");
    QSignalSpy spy1(widget, SIGNAL(requestSetLeftHand(const bool)));
    left->checkedChanged(left->checked());
    EXPECT_EQ(spy1.count(), 1);

    SwitchWidget *dis = widget->findChild<SwitchWidget *>("disInTyping");
    QSignalSpy spy2(widget, SIGNAL(requestSetDisTyping(const bool)));
    dis->checkedChanged(dis->checked());
    EXPECT_EQ(spy2.count(), 1);

    TitledSliderItem *scroll = widget->findChild<TitledSliderItem *>("scrollSpeed");
    QSignalSpy spy3(widget, SIGNAL(requestScrollSpeed(const int)));
    scroll->slider()->valueChanged(2);
    EXPECT_EQ(spy3.count(), 1);

    TitledSliderItem *dou = widget->findChild<TitledSliderItem *>("doubleClicked");
    QSignalSpy spy4(widget, SIGNAL(requestSetDouClick(const int)));
    dou->slider()->valueChanged(3);
    EXPECT_EQ(spy4.count(), 1);

    DouTestWidget *douTest = widget->findChild<DouTestWidget *>();
    DPictureSequenceView *pic = douTest->findChild<DPictureSequenceView *>();
    QMouseEvent event(QEvent::MouseButtonPress, QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(douTest, &event);
    pic->playEnd();

    QMouseEvent event2(QEvent::MouseButtonDblClick, QPoint(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QApplication::sendEvent(douTest, &event2);
    QApplication::sendEvent(douTest, &event);
    pic->playEnd();
    QApplication::sendEvent(douTest, &event2);
}
