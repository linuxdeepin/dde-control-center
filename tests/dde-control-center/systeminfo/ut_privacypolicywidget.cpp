#include "../src/frame/window/modules/systeminfo/privacypolicywidget.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dccV20::systeminfo;

class Test_PrivacyPolicyWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    PrivacyPolicyWidget *m_widget = nullptr;
};

void Test_PrivacyPolicyWidget::SetUp()
{

}

void Test_PrivacyPolicyWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_PrivacyPolicyWidget, full)
{
    m_widget = new PrivacyPolicyWidget();
    ASSERT_NE(m_widget, nullptr);

    EXPECT_NO_THROW(m_widget->onLinkActivated("\"https://www.uniontech.com/agreement/privacy-en\""));
}


