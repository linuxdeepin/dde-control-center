#include "kblayoutsettingwidget.h"
#include "keyboardmodel.h"

#include <QSignalSpy>
#include <DFloatingButton>
#include <QModelIndex>

#include "gtest/gtest.h"

using namespace dcc::keyboard;
using namespace dccV20::keyboard;

DWIDGET_USE_NAMESPACE

class Tst_KBLayoutSettingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    KBLayoutSettingWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_KBLayoutSettingWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new KBLayoutSettingWidget();
}

void Tst_KBLayoutSettingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_KBLayoutSettingWidget, emitSignal)
{
    widget->setModel(model);

    DFloatingButton *addLayout = widget->findChild<DFloatingButton *>("AddLayout");
    ASSERT_TRUE(nullptr != addLayout);
    QSignalSpy spyOnAddKeyBoard(widget, SIGNAL(layoutAdded(const QStringList &)));
    addLayout->clicked(true);
    EXPECT_EQ(spyOnAddKeyBoard.count(), 1);

    MyListView *myListView = widget->findChild<MyListView *>("KbLayoutListView");
    ASSERT_TRUE(nullptr != myListView);
}
