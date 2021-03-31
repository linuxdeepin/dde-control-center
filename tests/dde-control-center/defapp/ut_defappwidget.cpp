#include "../src/frame/window/modules/defapp/defappwidget.h"
#include "../src/frame/modules/defapp/defappworker.h"
#include "widgets/multiselectlistview.h"
#include "interface/namespace.h"

#include <DListView>
#include <QSignalSpy>
#include <QTest>

#include <gtest/gtest.h>

using namespace DCC_NAMESPACE::defapp;
using namespace dcc::defapp;
DWIDGET_USE_NAMESPACE

class Test_DefappWidget : public testing::Test
{
public:
    virtual void SetUp() override;
    virtual void TearDown() override;

public:
    DefaultAppsWidget *m_widget = nullptr;
};

void Test_DefappWidget::SetUp()
{
    m_widget = new DefaultAppsWidget;
}

void Test_DefappWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_DefappWidget, Init)
{
    ASSERT_NE(m_widget, nullptr);
}

TEST_F(Test_DefappWidget, FullTest)
{
    qRegisterMetaType<dcc::defapp::DefAppWorker::DefaultAppsCategory>("dcc::defapp::DefAppWorker::DefaultAppsCategory");
    QSignalSpy spy(m_widget, SIGNAL(requestCategoryClicked(dcc::defapp::DefAppWorker::DefaultAppsCategory)));
    dcc::widgets::MultiSelectListView *listview = m_widget->getCatView();
    listview->clicked(listview->model()->index(listview->model()->rowCount() - 1, 0));
    EXPECT_EQ(spy.count(), 1);
}
