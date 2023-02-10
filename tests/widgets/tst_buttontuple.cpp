//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>
#include <QPushButton>
#include "widgets/buttontuple.h"

using namespace DCC_NAMESPACE;

class Tst_ButtonTuple : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ButtonTuple;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ButtonTuple *obj = nullptr;
};

TEST_F(Tst_ButtonTuple, coverage)
{
    obj->setButtonType(ButtonTuple::ButtonTuple::Save);
    obj->leftButton()->setText("Cancel");
    obj->rightButton()->setText("Ok");
    obj->removeSpacing();
    obj->show();
}
