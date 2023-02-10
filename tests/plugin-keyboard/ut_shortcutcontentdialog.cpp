//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "src/plugin-keyboard/window/shortcutcontentdialog.h"
#include "src/plugin-keyboard/operation/shortcutmodel.h"

#include <QSignalSpy>
#include <QPushButton>

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_ShortcutContentDialog : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    ShortcutContentDialog *dialog = nullptr;
    ShortcutModel *model = nullptr;
};

void Tst_ShortcutContentDialog::SetUp()
{
    model = new ShortcutModel();
    dialog = new ShortcutContentDialog(model);
}

void Tst_ShortcutContentDialog::TearDown()
{
    delete model;
    model = nullptr;
    delete dialog;
    dialog = nullptr;
}

TEST_F(Tst_ShortcutContentDialog, emitSignal)
{
}
