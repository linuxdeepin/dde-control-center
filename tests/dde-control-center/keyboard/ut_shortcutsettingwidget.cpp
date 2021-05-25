#include "shortcutsettingwidget.h"
#include "shortcutmodel.h"

#include <QSignalSpy>
#include <DFloatingButton>

#include "gtest/gtest.h"

using namespace dcc::keyboard;
using namespace dccV20::keyboard;

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace keyboard {
class ShortcutModel;
class ShortcutItem;
}
}

class Tst_ShortCutSettingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    ShortCutSettingWidget *widget = nullptr;
    ShortcutModel *model = nullptr;
};

void Tst_ShortCutSettingWidget::SetUp()
{
    model = new ShortcutModel();
    widget = new ShortCutSettingWidget(model);
}

void Tst_ShortCutSettingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_ShortCutSettingWidget, emitSignal)
{
    DFloatingButton *btn = widget->findChild<DFloatingButton *>("AddCustomShortCut");
    QSignalSpy btnSpy(widget, SIGNAL(customShortcut()));
    btn->clicked(true);
    EXPECT_EQ(1, btnSpy.count());
}
