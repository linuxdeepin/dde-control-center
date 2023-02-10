//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "widgets/comboxwidget.h"

using namespace DCC_NAMESPACE;

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
