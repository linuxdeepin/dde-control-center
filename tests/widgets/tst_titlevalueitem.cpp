//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include "widgets/titlevalueitem.h"

using namespace DCC_NAMESPACE;

class Tst_TitleValueItem : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TitleValueItem();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TitleValueItem *obj = nullptr;
};

TEST_F(Tst_TitleValueItem, coverage)
{
    obj->setValue("aaa");
    obj->setTitle("ccc");
    obj->setWordWrap(0);
    obj->value();
}
