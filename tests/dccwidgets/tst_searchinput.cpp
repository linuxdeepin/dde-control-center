// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/searchinput.h"

using namespace dcc::widgets;

class Tst_SearchInput : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SearchInput();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SearchInput *obj = nullptr;
};

TEST_F(Tst_SearchInput, coverage)
{
    obj->setSearchText("aaa");
    obj->setIcon("bbb");
    obj->iconPath();
    obj->setIconVisible(false);
}
