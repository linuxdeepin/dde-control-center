//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-commoninfo/window/commonbackgrounditem.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

class Test_CommonBackgroundItem: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    CommonBackgroundItem *m_item = nullptr;
};

void Test_CommonBackgroundItem::SetUp()
{
    m_item = new CommonBackgroundItem();
}

void Test_CommonBackgroundItem::TearDown()
{
    delete m_item;
    m_item = nullptr;
}

TEST_F(Test_CommonBackgroundItem, full)
{
    EXPECT_NO_THROW(m_item->setThemeEnable(false));
    EXPECT_NO_THROW(m_item->updateBackground(QPixmap()));
}
