#include "interface/namespace.h"
#include "src/plugin-commoninfo/window/userexperienceprogramwidget.h"
#include "src/plugin-commoninfo/operation/commoninfomodel.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

class Test_UserExperienceProgramWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    UserExperienceProgramWidget *m_widget = nullptr;
    CommonInfoModel *m_model;
};

void Test_UserExperienceProgramWidget::SetUp()
{

}

void Test_UserExperienceProgramWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
    delete m_model;
    m_model = nullptr;
}

TEST_F(Test_UserExperienceProgramWidget, full)
{
    m_model = new CommonInfoModel();
    m_widget = new UserExperienceProgramWidget();
    ASSERT_NE(m_widget, nullptr);
    EXPECT_NO_THROW(m_widget->setModel(m_model));
    EXPECT_NO_THROW(m_widget->setDefaultUeProgram(false));
}


