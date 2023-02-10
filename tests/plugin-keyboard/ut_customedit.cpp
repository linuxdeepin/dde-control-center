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

#include "src/plugin-keyboard/operation/shortcutmodel.h"

#define private public
#include "src/plugin-keyboard/window/customedit.h"
#undef private

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_CustomEdit : public testing::Test
{
    void SetUp() override;
    void TearDown() override;

public:
    CustomEdit *edit = nullptr;
    ShortcutModel *model = nullptr;
};

void Tst_CustomEdit::SetUp()
{
    model = new ShortcutModel();
    edit = new CustomEdit(model);
}

void Tst_CustomEdit::TearDown()
{
    delete edit;
    edit = nullptr;

    delete model;
    model = nullptr;
}

TEST_F(Tst_CustomEdit, CustomEdit)
{
    ShortcutInfo info;
    info.id = 1;
    info.name = "test";
    info.accels = "Control+W";
    EXPECT_NO_THROW(edit->setShortcut(&info));
    EXPECT_NO_THROW(edit->setBottomTip(&info));
    EXPECT_NO_THROW(edit->onSaveAccels());
    EXPECT_NO_THROW(edit->keyEvent(true, "Control+W"));
}
