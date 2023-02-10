//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "src/plugin-keyboard/window/searchinput.h"

#include <QDebug>

#include "gtest/gtest.h"

#include <iostream>

using namespace DCC_NAMESPACE;

class Tst_SearchInput : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
//    GeneralKBSettingWidget *widget = nullptr;
    SearchInput *view = nullptr;
};

void Tst_SearchInput::SetUp()
{
    view = new SearchInput();
}

void Tst_SearchInput::TearDown()
{
    delete view;
    view = nullptr;
}

TEST_F(Tst_SearchInput, SearchInput)
{
    EXPECT_NO_THROW(view->setSearchText("test"));
    EXPECT_NO_THROW(view->setIconVisible(true));
    EXPECT_NO_THROW(view->setIcon("aaa"));
    EXPECT_EQ("aaa", view->iconPath());
}
