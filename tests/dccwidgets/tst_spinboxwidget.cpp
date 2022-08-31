// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/spinboxwidget.h"

using namespace dcc::widgets;

class Tst_SpinBoxWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new SpinBoxWidget();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    SpinBoxWidget *obj = nullptr;
};

TEST_F(Tst_SpinBoxWidget, coverage)
{
    obj->spinBox();
    obj->setTitle("bbb");
    obj->setDefaultVal(1);
}
