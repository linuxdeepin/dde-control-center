#include "../src/frame/window/modules/systeminfo/userlicensewidget.h"
#include "../src/frame/window/protocolfile.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>
#include <QtConcurrent>

DWIDGET_USE_NAMESPACE
using namespace dccV20::systeminfo;

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
