//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "gtest/gtest.h"

#include "src/frame/mainwindow.h"

using namespace DCC_NAMESPACE;

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