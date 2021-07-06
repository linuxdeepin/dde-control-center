#include "systemlanguagewidget.h"
#include "keyboardmodel.h"

#include <QSignalSpy>
#include <QModelIndex>
#include <DFloatingButton>

#include "gtest/gtest.h"

using namespace dcc::keyboard;
using namespace dccV20::keyboard;

DWIDGET_USE_NAMESPACE

class Tst_SystemLanguageWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    SystemLanguageWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_SystemLanguageWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new SystemLanguageWidget(model);
}

void Tst_SystemLanguageWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_SystemLanguageWidget, emitSignal)
{
    DFloatingButton *btn = widget->findChild<DFloatingButton *>("AddSystemLanguage");
    QSignalSpy btnSpy(widget, SIGNAL(onSystemLanguageAdded()));
    btn->clicked(true);
    EXPECT_EQ(1, btnSpy.count());
}
