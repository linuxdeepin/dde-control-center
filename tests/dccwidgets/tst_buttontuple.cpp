// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/buttontuple.h"

using namespace dcc::widgets;

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
    obj->leftButton();
    obj->rightButton();
    obj->removeSpacing();
}
