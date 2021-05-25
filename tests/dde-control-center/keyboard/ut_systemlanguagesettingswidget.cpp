#include "systemlanguagesettingwidget.h"
#include "keyboardmodel.h"

#include <QSignalSpy>
#include <QPushButton>

#include "gtest/gtest.h"

using namespace dcc::keyboard;
using namespace dccV20::keyboard;

DWIDGET_USE_NAMESPACE

class Tst_SystemLanguageSettingWidget : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    SystemLanguageSettingWidget *widget = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_SystemLanguageSettingWidget::SetUp()
{
    model = new KeyboardModel();
    widget = new SystemLanguageSettingWidget(model);
}

void Tst_SystemLanguageSettingWidget::TearDown()
{
    delete model;
    model = nullptr;
    delete widget;
    widget = nullptr;
}

TEST_F(Tst_SystemLanguageSettingWidget, emitSignal)
{
    QPushButton *okBtn = widget->findChild<QPushButton *>("Ok");
    QSignalSpy okBtnSpy(widget, SIGNAL(back()));
    okBtn->clicked(true);
    EXPECT_EQ(1, okBtnSpy.count());

    QPushButton *cancelBtn = widget->findChild<QPushButton *>("Cancel");
    QSignalSpy cancelBtnSpy(widget, SIGNAL(back()));
    cancelBtn->clicked(true);
    EXPECT_EQ(1, cancelBtnSpy.count());
}
