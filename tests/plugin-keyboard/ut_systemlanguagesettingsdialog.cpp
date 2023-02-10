//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "src/plugin-keyboard/window/systemlanguagesettingdialog.h"
#include "src/plugin-keyboard/operation/keyboardmodel.h"

#include <QSignalSpy>
#include <QPushButton>

#include "gtest/gtest.h"

using namespace DCC_NAMESPACE;
DWIDGET_USE_NAMESPACE

class Tst_SystemLanguageSettingDialog : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    SystemLanguageSettingDialog *dialog = nullptr;
    KeyboardModel *model = nullptr;
};

void Tst_SystemLanguageSettingDialog::SetUp()
{
    model = new KeyboardModel();
    dialog = new SystemLanguageSettingDialog(model);
}

void Tst_SystemLanguageSettingDialog::TearDown()
{
    delete model;
    model = nullptr;
    delete dialog;
    dialog = nullptr;
}

TEST_F(Tst_SystemLanguageSettingDialog, emitSignal)
{
    QPushButton *okBtn = dialog->findChild<QPushButton *>("Ok");
    QSignalSpy okBtnSpy(dialog, SIGNAL(requestCloseDlg()));
    okBtn->clicked(true);
    EXPECT_EQ(1, okBtnSpy.count());

    QPushButton *cancelBtn = dialog->findChild<QPushButton *>("Cancel");
    QSignalSpy cancelBtnSpy(dialog, SIGNAL(requestCloseDlg()));
    cancelBtn->clicked(true);
    EXPECT_EQ(1, cancelBtnSpy.count());
}
