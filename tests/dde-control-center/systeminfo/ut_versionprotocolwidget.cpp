#include "../src/frame/window/modules/systeminfo/versionprotocolwidget.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dccV20::systeminfo;

class Test_VersionProtocolWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    VersionProtocolWidget *m_widget = nullptr;
};

void Test_VersionProtocolWidget::SetUp()
{

}

void Test_VersionProtocolWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_VersionProtocolWidget, test1)
{
    VersionProtocolWidget *m_widget  = new VersionProtocolWidget();
    ASSERT_EQ(m_widget->isVisible(), false) ;

    QSignalSpy spy1(m_widget, SIGNAL(loadTextFinished()));
    m_widget->loadTextFinished();
    EXPECT_EQ(spy1.count(), 1);
}