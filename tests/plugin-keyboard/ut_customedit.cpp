//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
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
    info.id = "1";
    info.name = "test";
    info.accels = "Control+W";
    EXPECT_NO_THROW(edit->setShortcut(&info));
    EXPECT_NO_THROW(edit->setBottomTip(&info));
    EXPECT_NO_THROW(edit->onSaveAccels());
    EXPECT_NO_THROW(edit->keyEvent(true,"Control+W"));
}
