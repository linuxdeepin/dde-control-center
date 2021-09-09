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



TEST_F(Tst_SystemLanguageWidget, widget)
{
    QStringList la;
    QStringList strList;
    la.append("zh_CN.UTF-8");
    la.append("ja_JP.UTF-8");
    la.append("zh_HK.UTF-8");
    strList.append("简体中文");
    strList.append("日本語");
    strList.append("繁體中文(香港)");
    strList.append("British English");
    model->setLocaleLang(la);
    emit model->curLocalLangChanged(strList);
    widget->onDefault("简体中文");
    widget->onSetCurLang(0);
    DCommandLinkButton *btnEdit= widget->findChild<DCommandLinkButton *>("Edit");
    btnEdit->click();
    btnEdit->click();
    DFloatingButton *btn = widget->findChild<DFloatingButton *>("AddSystemLanguage");
    QSignalSpy btnSpy(widget, SIGNAL(onSystemLanguageAdded()));
    btn->clicked(true);
    EXPECT_EQ(1, btnSpy.count());

}
