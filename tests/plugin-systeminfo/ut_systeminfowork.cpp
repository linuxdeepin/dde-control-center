//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-systeminfo/operation/systeminfomodel.h"

#define private public
#include "src/plugin-systeminfo/operation/systeminfowork.h"
#undef private

#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace DCC_NAMESPACE;
class Test_SystemInfoWork: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    SystemInfoWork *m_work = nullptr;
    SystemInfoModel *m_model = nullptr;
};

void Test_SystemInfoWork::SetUp()
{
    m_model = new SystemInfoModel;
    m_work = new SystemInfoWork(m_model);
}

void Test_SystemInfoWork::TearDown()
{
    delete m_work;
    m_work = nullptr;

    delete m_model;
    m_model = nullptr;
}


TEST_F(Test_SystemInfoWork, full)
{
    EXPECT_NO_THROW(m_work->activate());
}
