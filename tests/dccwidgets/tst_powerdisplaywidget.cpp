// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/powerdisplaywidget.h"

using namespace dcc::widgets;

class Tst_PowerDisplayWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new PowerDisplayWidget("aaa");
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    PowerDisplayWidget *obj = nullptr;
};

TEST_F(Tst_PowerDisplayWidget, coverage)
{
    obj->setText("aaa");
    obj->setTitle("bbb");
}
