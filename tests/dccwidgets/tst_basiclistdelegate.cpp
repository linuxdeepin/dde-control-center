// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include "../../include/widgets/basiclistdelegate.h"

using namespace dcc::widgets;

class Tst_BasicListDelegate : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BasicListDelegate;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BasicListDelegate *obj = nullptr;
};
