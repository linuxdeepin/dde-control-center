#include "../src/frame/window/modules/defapp/defappwidget.h"
#include "../src/frame/window/modules/defapp/defappdetailwidget.h"
#include "../src/frame/modules/defapp/defappworker.h"
#include "../src/frame/modules/defapp/defappmodel.h"
#include "interface/namespace.h"

#include <DListView>
#include <DFloatingButton>
#include <QSignalSpy>
#include <QTest>
#include <gtest/gtest.h>

using namespace DCC_NAMESPACE::defapp;
using namespace dcc::defapp;
DWIDGET_USE_NAMESPACE

class Test_DefappDetailWidget : public testing::Test
{
public:
    virtual void SetUp() override;
    virtual void TearDown() override;

public:
    DefappDetailWidget *m_widget = nullptr;
};

void Test_DefappDetailWidget::SetUp()
{
    m_widget = new DefappDetailWidget(dcc::defapp::DefAppWorker::Browser);
}

void Test_DefappDetailWidget::TearDown()
{
    delete m_widget;
    m_widget = nullptr;
}

TEST_F(Test_DefappDetailWidget, Init)
{
    ASSERT_NE(m_widget, nullptr);
}

TEST_F(Test_DefappDetailWidget, FullTest)
{
    DefAppModel model;
    DefAppWorker worker(&model);
    worker.active();
    worker.onGetListApps();

    QList<QDBusPendingCallWatcher *> watcherList = worker.findChildren<QDBusPendingCallWatcher *>();
    foreach (auto watcher, watcherList) {
        watcher->finished(watcher);
        QTest::qWait(10);
    }

    worker.onSetDefaultApp("Mail", model.getModMail()->getDefault());
    QTest::qWait(20);

    m_widget->setModel(&model);
    qRegisterMetaType<dcc::defapp::App>("dcc::defapp::App");
    QSignalSpy spy(m_widget, SIGNAL(requestSetDefaultApp(const QString &, const dcc::defapp::App &)));
    DListView *applist = m_widget->getAppListview();
    applist->setCurrentIndex(applist->model()->index(applist->model()->rowCount() - 1, 0));
    applist->clicked(applist->model()->index(applist->model()->rowCount() - 1, 0));
    EXPECT_EQ(spy.count(), 1);
}
