//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"

#define private public
#include "src/plugin-commoninfo/operation/commoninfowork.h"
#undef private

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace DCC_NAMESPACE;
class Test_CommonInfoWork: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    CommonInfoWork *m_work = nullptr;
    CommonInfoModel *m_model = nullptr;
};

void Test_CommonInfoWork::SetUp()
{
    m_model = new CommonInfoModel;
    m_work = new CommonInfoWork(m_model);
}

void Test_CommonInfoWork::TearDown()
{
    delete m_work;
    m_work = nullptr;

    delete m_model;
    m_model = nullptr;
}

TEST_F(Test_CommonInfoWork, setBootDelaySignal)
{
    EXPECT_NO_THROW(m_model->bootDelay());
    EXPECT_NO_THROW(m_work->setBootDelay(false));
}

TEST_F(Test_CommonInfoWork, setEnableThemeSignal)
{
    EXPECT_NO_THROW(m_model->themeEnabled());
    EXPECT_NO_THROW(m_work->setEnableTheme(false));
}

TEST_F(Test_CommonInfoWork, setDefaultEntrySignal)
{
    EXPECT_NO_THROW(m_model->defaultEntry());
    EXPECT_NO_THROW(m_work->setDefaultEntry("Uniontech OS Desktop 20 Pro GNU/Linux"));
}


TEST_F(Test_CommonInfoWork, full)
{
    EXPECT_NO_THROW(m_work->disableGrubEditAuth());
    EXPECT_NO_THROW(m_work->active());
}
