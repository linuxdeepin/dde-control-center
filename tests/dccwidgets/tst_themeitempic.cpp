// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/themeitempic.h"

using namespace dcc::widgets;

class Tst_ThemeItemPic : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ThemeItemPic();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ThemeItemPic *obj = nullptr;
};

TEST_F(Tst_ThemeItemPic, coverage)
{
    obj->setPicPath("aaa");
    obj->paintEvent(nullptr);
}
