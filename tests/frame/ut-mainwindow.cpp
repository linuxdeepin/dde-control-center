#include "gtest/gtest.h"

#include "src/frame/mainwindow.h"

DCC_USE_NAMESPACE

class Tst_MainWindow : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new MainWindow;
    }
    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }
public:
    MainWindow *obj = nullptr;
};

TEST_F(Tst_MainWindow, coverage)
{
    obj->showPage("firstmenu1/menu3", MainWindow::UrlType::Name);
}