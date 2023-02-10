//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#define private public
#include "src/plugin-keyboard/window/customitem.h"
#undef private

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE
class Tst_CustomItem : public testing::Test
{
    void SetUp() override;
    void TearDown() override;

public:
    CustomItem *item = nullptr;
};

void Tst_CustomItem::SetUp()
{
    item = new CustomItem();
}

void Tst_CustomItem::TearDown()
{
    delete item;
    item = nullptr;
}

TEST_F(Tst_CustomItem, CustomItem)
{
    EXPECT_NO_THROW(item->setTitle("test"));
    EXPECT_NO_THROW(item->setShortcut("Control+W"));
    EXPECT_EQ("Control+W", item->text());
    EXPECT_NO_THROW(item->setAlert(true));
}
