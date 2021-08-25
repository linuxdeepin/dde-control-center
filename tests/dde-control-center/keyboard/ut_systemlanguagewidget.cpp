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

TEST_F(Tst_SystemLanguageWidget, onEditClicked)
{
    EXPECT_NO_THROW(widget->onEditClicked());
}

TEST_F(Tst_SystemLanguageWidget, onAddLanguage)
{
    widget->onAddLanguage("简体中文");
    widget->onSetCurLang(0);
    widget->onDefault("简体中文");
    QSignalSpy modelSpy(model, SIGNAL(curLocalLangChanged(const QStringList)));
    QStringList sl = (QStringList() << "简体中文" << "American English");
    model->curLocalLangChanged(sl);
    EXPECT_EQ(1, modelSpy.count());
}
