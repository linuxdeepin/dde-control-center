//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"
#include "src/plugin-keyboard/operation/keyboarddbusproxy.h"

#define private public
#include "src/plugin-keyboard/operation/keyboardwork.h"
#undef private

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace DCC_NAMESPACE;
class Tst_KeyboardWorker: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    KeyboardWorker *m_work = nullptr;
    KeyboardModel *m_model = nullptr;
};

void Tst_KeyboardWorker::SetUp()
{
    m_model = new KeyboardModel;
    m_work = new KeyboardWorker(m_model);
}

void Tst_KeyboardWorker::TearDown()
{
    delete m_work;
    m_work = nullptr;

    delete m_model;
    m_model = nullptr;
}

TEST_F(Tst_KeyboardWorker, full)
{
    EXPECT_NO_THROW(m_work->active());
}

//TEST_F(Tst_KeyboardWorker, setRepeatDelaySignal)
//{
//    uint value = m_model->repeatDelay();
//    m_work->setRepeatDelay(value + 1);
//    EXPECT_EQ(value + 1, m_model->repeatDelay());
//}

//TEST_F(Tst_KeyboardWorker, setRepeatIntervalSignal)
//{
//    uint value = m_model->repeatInterval();
//    m_work->setRepeatInterval(value + 1);
//    EXPECT_EQ(value + 1, m_model->repeatInterval());
//}

//TEST_F(Tst_KeyboardWorker, setNumLockSignal)
//{
//    bool value = m_model->numLock();
//    m_work->setNumLock(!value);
//    EXPECT_EQ(!value, m_model->numLock());
//}

//TEST_F(Tst_KeyboardWorker, setCapsLockSignal)
//{
//    bool value = m_model->capsLock();
//    m_work->setCapsLock(!value);
//    EXPECT_EQ(!value, m_model->capsLock());
//}

