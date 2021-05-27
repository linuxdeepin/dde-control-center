#include "../src/frame/window/modules/systeminfo/systeminfowidget.h"
#include "../src/frame/window/modules/systeminfo/systeminfomodule.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE
using namespace dccV20::systeminfo;

class Test_SysteminfoWidget: public testing::Test
{
public:
    virtual void SetUp() override;

    virtual void TearDown() override;

public:
    SystemInfoWidget *m_widget = nullptr;
};

void Test_SysteminfoWidget::SetUp()
{
    m_widget = new SystemInfoWidget();
}

void Test_SysteminfoWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_SysteminfoWidget, test1)
{
    ASSERT_EQ(1, 1);
}

TEST_F(Test_SysteminfoWidget, full)
{
    ASSERT_EQ(1, 1);

    DListView *listView = m_widget->getSystemListViewPointer();
    ASSERT_NE(listView, nullptr);

    QSignalSpy spy1(m_widget, SIGNAL(requestShowAboutNative()));
    m_widget->showDefaultWidget();
    EXPECT_EQ(spy1.count(), 1);
    listView->clicked(listView->model()->index(1, 0));
    listView->clicked(listView->model()->index(0, 0));
    EXPECT_EQ(spy1.count(), 2);

    QSignalSpy spy2(m_widget, SIGNAL(requestShowVersionProtocol()));
    listView->clicked(listView->model()->index(1, 0));
    EXPECT_EQ(spy2.count(), 1);

    QSignalSpy spy3(m_widget, SIGNAL(requestShowEndUserLicenseAgreement()));
    listView->clicked(listView->model()->index(2, 0));
    EXPECT_EQ(spy3.count(), 1);

    QSignalSpy spy4(m_widget, SIGNAL(requestShowPrivacyPolicy()));
    listView->clicked(listView->model()->index(3, 0));
    EXPECT_EQ(spy4.count(), 1);

    int re = m_widget->showPath("About This PC");
    EXPECT_EQ(re, 0);
    EXPECT_EQ(spy1.count(), 3);
}
