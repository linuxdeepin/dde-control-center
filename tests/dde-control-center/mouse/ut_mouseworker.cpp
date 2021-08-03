#include <gtest/gtest.h>
#include <QProcess>

#include "interface/namespace.h"
#include "../src/frame/modules/mouse/mouseworker.h"

using namespace dcc::mouse;

class Tst_MouseWorker : public testing::Test
{
public:
    void SetUp() override
    {
        model = new MouseModel;
        worker = new MouseWorker(model);
    }

    void TearDown() override
    {
        delete worker;
        worker = nullptr;
        delete model;
        model = nullptr;
    }

public:
    MouseWorker *worker = nullptr;
    MouseModel *model = nullptr;
};

TEST_F(Tst_MouseWorker, emitSignal)
{
    // 模拟鼠标dbus发送信号
    bool mouseExist = model->mouseExist();
    worker->setMouseExist(!mouseExist);
    EXPECT_EQ(!mouseExist, model->mouseExist());

    bool leftHandState = model->leftHandState();
    worker->setLeftHandState(!leftHandState);
    worker->onLeftHandStateChanged(!leftHandState);
    EXPECT_EQ(!leftHandState, model->leftHandState());

    bool mouseNaturalScroll = model->mouseNaturalScroll();
    worker->setMouseNaturalScrollState(!mouseNaturalScroll);
    worker->onMouseNaturalScrollStateChanged(mouseNaturalScroll);
    EXPECT_EQ(!mouseNaturalScroll, model->mouseNaturalScroll());

    int doubleSpeed = model->doubleSpeed();
    worker->setDouClick(800 - (++doubleSpeed) * 100);
    worker->onDouClickChanged(doubleSpeed);
    EXPECT_EQ(doubleSpeed, model->doubleSpeed());

    bool disTpad = model->disTpad();
    worker->setDisTouchPad(!disTpad);
    worker->onDisTouchPadChanged(!disTpad);
    EXPECT_EQ(!disTpad, model->disTpad());

    bool accelProfile = model->accelProfile();
    worker->setAccelProfile(!accelProfile);
    worker->onAccelProfileChanged(!accelProfile);
    EXPECT_EQ(!accelProfile, model->accelProfile());

    model->setMouseMoveSpeed(5);
    worker->setMouseMotionAcceleration(0.6);
    worker->onMouseMotionAccelerationChanged(4);
    EXPECT_EQ(4, model->mouseMoveSpeed());

    // 模拟触控板dbus发送信号
    bool tpadExist = model->tpadExist();
    worker->setTpadExist(!tpadExist);
    EXPECT_EQ(!tpadExist, model->tpadExist());

    bool touchNaturalScrollState = model->tpadNaturalScroll();
    worker->setTouchNaturalScrollState(!touchNaturalScrollState);
    worker->onTouchNaturalScrollStateChanged(!touchNaturalScrollState);
    EXPECT_EQ(!touchNaturalScrollState, model->tpadNaturalScroll());

    bool disIfTyping = model->disIfTyping();
    worker->setDisTyping(!disIfTyping);
    worker->onDisTypingChanged(!disIfTyping);
    EXPECT_EQ(!disIfTyping, model->disIfTyping());

    worker->setDouClick(800 - (++doubleSpeed) * 100);
    EXPECT_EQ(doubleSpeed, model->doubleSpeed());

    model->setTpadMoveSpeed(5);
    worker->setTouchpadMotionAcceleration(0.6);
    worker->onTouchpadMotionAccelerationChanged(4);
    EXPECT_EQ(4, model->tpadMoveSpeed());

    bool tapClick = model->tapclick();
    worker->setTapClick(!tapClick);
    worker->onTapClick(!tapClick);
    EXPECT_EQ(!tapClick, model->tapclick());

    bool palmDetect = model->palmDetect();
    worker->setPalmDetect(!palmDetect);
    worker->onPalmDetectChanged(!palmDetect);
    EXPECT_EQ(!palmDetect, model->palmDetect());

    int palmMinWidth = model->palmMinWidth();
    worker->setPalmMinWidth(++palmMinWidth);
    worker->onPalmMinWidthChanged(palmMinWidth);
    EXPECT_EQ(palmMinWidth, model->palmMinWidth());

    int palmMinz = model->palmMinz();
    worker->setPalmMinz(++palmMinz);
    worker->onPalmMinzChanged(palmMinz);
    EXPECT_EQ(palmMinz, model->palmMinz());

    // 模拟红点发送信号
    bool redPointExist = model->redPointExist();
    worker->setRedPointExist(!redPointExist);
    EXPECT_EQ(!redPointExist, model->redPointExist());

    model->setRedPointMoveSpeed(5);
    worker->setTrackPointMotionAcceleration(0.6);
    worker->onTrackPointMotionAccelerationChanged(4);
    EXPECT_EQ(4, model->redPointMoveSpeed());

    // 模拟硬件设备发送信号
    uint scrollSpeed = model->scrollSpeed();
    worker->setScrollSpeed(++scrollSpeed);
    worker->onScrollSpeedChanged(static_cast<int>(scrollSpeed));
    EXPECT_EQ(scrollSpeed, model->scrollSpeed());
}
