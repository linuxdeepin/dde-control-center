//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#define private public
#include "src/plugin-keyboard/window/titlebuttonItem.h"
#undef private

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
class Tst_TitleButtonItem : public testing::Test
{
    void SetUp() override;
    void TearDown() override;

public:
    TitleButtonItem *item = nullptr;
};

void Tst_TitleButtonItem::SetUp()
{
    item = new TitleButtonItem();
}

void Tst_TitleButtonItem::TearDown()
{
    delete item;
    item = nullptr;
}

TEST_F(Tst_TitleButtonItem, CustomItem)
{
    EXPECT_NO_THROW(item->setTitle("test"));
    EXPECT_NO_THROW(item->setValue("Control+W"));
    EXPECT_NO_THROW(item->updateTitleSize());
}
