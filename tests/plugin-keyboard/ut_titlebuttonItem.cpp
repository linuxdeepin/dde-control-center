/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#define private public
#include "src/plugin-keyboard/window/titlebuttonItem.h"
#undef private

#include "gtest/gtest.h"

DCC_USE_NAMESPACE
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
