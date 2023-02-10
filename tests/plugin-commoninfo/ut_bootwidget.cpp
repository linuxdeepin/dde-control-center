//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-commoninfo/window/bootwidget.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>
#include <QtConcurrent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
class Test_BootWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    BootWidget *m_widget  = nullptr;
};

void Test_BootWidget::SetUp()
{

}

void Test_BootWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_BootWidget, fulltest)
{
    BootWidget *m_widget  = new BootWidget();
    ASSERT_EQ(m_widget->isVisible(), false) ;
}
