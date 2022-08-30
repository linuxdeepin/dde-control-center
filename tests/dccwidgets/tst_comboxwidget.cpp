// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/comboxwidget.h"

using namespace dcc::widgets;

class Tst_ComboxWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ComboxWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ComboxWidget *obj = nullptr;
};

TEST_F(Tst_ComboxWidget, coverage)
{
    obj->setComboxOption(QStringList() << "aaa" << "bbb" << "ccc");
    obj->setCurrentText("ccc");
    obj->setTitle("ddd");
    obj->comboBox();
}
