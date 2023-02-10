//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#define private public
#include "src/plugin-keyboard/window/shortcutitem.h"
#undef private

#include "gtest/gtest.h"
#include <QtTest/QtTest>

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_ShortcutItem : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    ShortcutItem *item = nullptr;
};

void Tst_ShortcutItem::SetUp()
{
    item = new ShortcutItem();
}

void Tst_ShortcutItem::TearDown()
{
    delete item;
    item = nullptr;
}

TEST_F(Tst_ShortcutItem, item)
{
    EXPECT_NO_THROW(item->setTitle("test"));
    EXPECT_NO_THROW(item->setShortcut("<Control><Alt>T"));
    EXPECT_NO_THROW(item->onEditMode(true));
    EXPECT_NO_THROW(item->onEditMode(false));
    EXPECT_NO_THROW(item->setConfigName("test"));
    EXPECT_EQ(item->configName(), "test");
    item->releaseMouse();
}
