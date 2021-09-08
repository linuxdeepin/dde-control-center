#define private public
#include "../src/frame/window/modules/systeminfo/systeminfowidget.h"
#undef private

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

TEST_F(Test_SysteminfoWidget, initData)
{
    EXPECT_NO_THROW(m_widget->initData());
}

TEST_F(Test_SysteminfoWidget, initWidget)
{
    EXPECT_NO_THROW(m_widget->initWidget());
}


TEST_F(Test_SysteminfoWidget, requestShowAboutNative)
{
    DListView *listView = m_widget->getSystemListViewPointer();
    ASSERT_NE(listView, nullptr);

    QSignalSpy spy1(m_widget, SIGNAL(requestShowAboutNative()));
    m_widget->showDefaultWidget();
    EXPECT_EQ(spy1.count(), 1);
    listView->clicked(listView->model()->index(1, 0));
    listView->clicked(listView->model()->index(0, 0));
    EXPECT_EQ(spy1.count(), 2);
}

TEST_F(Test_SysteminfoWidget, requestShowVersionProtocol)
{
    DListView *listView = m_widget->getSystemListViewPointer();
    ASSERT_NE(listView, nullptr);

    QSignalSpy spy2(m_widget, SIGNAL(requestShowVersionProtocol()));
    listView->clicked(listView->model()->index(1, 0));
    EXPECT_EQ(spy2.count(), 1);
}

TEST_F(Test_SysteminfoWidget, requestShowEndUserLicenseAgreement)
{
    DListView *listView = m_widget->getSystemListViewPointer();
    ASSERT_NE(listView, nullptr);

    QSignalSpy spy3(m_widget, SIGNAL(requestShowEndUserLicenseAgreement()));
    listView->clicked(listView->model()->index(2, 0));
    EXPECT_EQ(spy3.count(), 1);
}

TEST_F(Test_SysteminfoWidget, requestShowPrivacyPolicy)
{
    DListView *listView = m_widget->getSystemListViewPointer();
    ASSERT_NE(listView, nullptr);


    QSignalSpy spy4(m_widget, SIGNAL(requestShowPrivacyPolicy()));
    QModelIndex index1 = listView->model()->index(3, 0);
    listView->activated(index1);
    EXPECT_EQ(spy4.count(), 1);

    EXPECT_NO_THROW(m_widget->onListClicked(index1));
}


TEST_F(Test_SysteminfoWidget, full)
{
    int re = m_widget->showPath("About This PC");
    EXPECT_EQ(re, 0);

    EXPECT_NO_THROW(m_widget->showDefaultWidget());

    EXPECT_NO_THROW(m_widget->showPath("test"));

    EXPECT_NO_THROW(m_widget->setCurrentIndex(1));
}
