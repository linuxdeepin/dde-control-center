//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "interface/namespace.h"
#include "src/plugin-systeminfo/window/userlicensewidget.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>
#include <QtConcurrent>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;
class Test_UserLicenseWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    UserLicenseWidget *m_widget  = nullptr;
};

void Test_UserLicenseWidget::SetUp()
{

}

void Test_UserLicenseWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_UserLicenseWidget, fulltest)
{
    UserLicenseWidget *m_widget  = new UserLicenseWidget();
    ASSERT_EQ(m_widget->isVisible(), false) ;
}
