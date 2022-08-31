// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/loadingindicator.h"

using namespace dcc::widgets;

class Tst_LoadingIndicator : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new LoadingIndicator;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    LoadingIndicator *obj = nullptr;
};

TEST_F(Tst_LoadingIndicator, coverage)
{
    obj->theme();
    obj->setTheme("bbb");
}
