
#include "interface/namespace.h"
#include "src/plugin-mouse/operation/mousemodel.h"

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DCC_USE_NAMESPACE
class Tst_MouseModel : public testing::Test
{
public:
    void SetUp() override
    {
        m_model = new MouseModel;
    }

    void TearDown() override
    {
        delete m_model;
        m_model = nullptr;
    }

public:
    MouseModel *m_model = nullptr;
};

TEST_F(Tst_MouseModel, setLeftHandState)
{
    m_model->setLeftHandState(false);
    QSignalSpy spy1(m_model, SIGNAL(leftHandStateChanged(const bool)));
    m_model->setLeftHandState(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->leftHandState();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setDisIfTyping)
{
    m_model->setDisIfTyping(false);
    QSignalSpy spy1(m_model, SIGNAL(disIfTypingStateChanged(const bool)));
    m_model->setDisIfTyping(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->disIfTyping();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setTpadExist)
{
    m_model->setTpadExist(false);
    QSignalSpy spy1(m_model, SIGNAL(tpadExistChanged(const bool)));
    m_model->setTpadExist(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->tpadExist();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setMouseExist)
{
    m_model->setMouseExist(false);
    QSignalSpy spy1(m_model, SIGNAL(mouseExistChanged(bool)));
    m_model->setMouseExist(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->mouseExist();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setRedPointExist)
{
    m_model->setRedPointExist(false);
    QSignalSpy spy1(m_model, SIGNAL(redPointExistChanged(const bool)));
    m_model->setRedPointExist(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->redPointExist();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setDoubleSpeed)
{
    m_model->setDoubleSpeed(1);
    QSignalSpy spy1(m_model, SIGNAL(doubleSpeedChanged(int)));
    m_model->setDoubleSpeed(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->doubleSpeed();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setMouseNaturalScroll)
{
    m_model->setMouseNaturalScroll(false);
    QSignalSpy spy1(m_model, SIGNAL(mouseNaturalScrollChanged(const bool)));
    m_model->setMouseNaturalScroll(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->mouseNaturalScroll();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setTpadNaturalScroll)
{
    m_model->setTpadNaturalScroll(false);
    QSignalSpy spy1(m_model, SIGNAL(tpadNaturalScrollChanged(const bool)));
    m_model->setTpadNaturalScroll(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->tpadNaturalScroll();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setMouseMoveSpeed)
{
    m_model->setMouseMoveSpeed(1);
    QSignalSpy spy1(m_model, SIGNAL(mouseMoveSpeedChanged(int)));
    m_model->setMouseMoveSpeed(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->mouseMoveSpeed();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setTpadMoveSpeed)
{
    m_model->setTpadMoveSpeed(1);
    QSignalSpy spy1(m_model, SIGNAL(tpadMoveSpeedChanged(int)));
    m_model->setTpadMoveSpeed(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->tpadMoveSpeed();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setAccelProfile)
{
    m_model->setAccelProfile(false);
    QSignalSpy spy1(m_model, SIGNAL(accelProfileChanged(bool)));
    m_model->setAccelProfile(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->accelProfile();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setDisTpad)
{
    m_model->setDisTpad(false);
    QSignalSpy spy1(m_model, SIGNAL(disTpadChanged(const bool)));
    m_model->setDisTpad(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->disTpad();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setRedPointMoveSpeed)
{
    m_model->setRedPointMoveSpeed(1);
    QSignalSpy spy1(m_model, SIGNAL(redPointMoveSpeedChanged(int)));
    m_model->setRedPointMoveSpeed(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->redPointMoveSpeed();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setPalmDetect)
{
    m_model->setPalmDetect(false);
    QSignalSpy spy1(m_model, SIGNAL(palmDetectChanged(const bool)));
    m_model->setPalmDetect(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->palmDetect();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setPalmMinWidth)
{
    m_model->setPalmMinWidth(1);
    QSignalSpy spy1(m_model, SIGNAL(palmMinWidthChanged(int)));
    m_model->setPalmMinWidth(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->palmMinWidth();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setPalmMinz)
{
    m_model->setPalmMinz(1);
    QSignalSpy spy1(m_model, SIGNAL(palmMinzChanged(int)));
    m_model->setPalmMinz(2);
    EXPECT_EQ(spy1.count(), 1);
    int value = m_model->palmMinz();
    EXPECT_EQ(value, 2);
}

TEST_F(Tst_MouseModel, setTapClick)
{
    m_model->setTapClick(false);
    QSignalSpy spy1(m_model, SIGNAL(tapClickChanged(const bool)));
    m_model->setTapClick(true);
    EXPECT_EQ(spy1.count(), 1);
    bool value = m_model->tapclick();
    EXPECT_EQ(value, true);
}

TEST_F(Tst_MouseModel, setScrollSpeed)
{
    m_model->setScrollSpeed(1);
    QSignalSpy spy1(m_model, SIGNAL(scrollSpeedChanged(uint)));
    m_model->setScrollSpeed(2);
    EXPECT_EQ(spy1.count(), 1);
    uint value = m_model->scrollSpeed();
    EXPECT_EQ(value, 2);
}
