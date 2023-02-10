//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "src/plugin-keyboard/window/keylabel.h"
#include "gtest/gtest.h"

class Tst_KeyLabel : public testing::Test
{
    void SetUp() override;

    void TearDown() override;

public:
    KeyLabel *keyLabel = nullptr;
};

void Tst_KeyLabel::SetUp()
{
    keyLabel = new KeyLabel("test");
}

void Tst_KeyLabel::TearDown()
{
    delete keyLabel;
    keyLabel = nullptr;
}

TEST_F(Tst_KeyLabel, KeyLabel)
{
    EXPECT_NO_THROW(keyLabel->setEnter(true));
}
