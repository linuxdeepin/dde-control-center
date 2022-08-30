// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/editablenextpagewidget.h"

using namespace dcc::widgets;

class Tst_EditableNextPageWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new EditableNextPageWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    EditableNextPageWidget *obj = nullptr;
};

TEST_F(Tst_EditableNextPageWidget, coverage)
{
    obj->setTitle("aaa");
    obj->setIcon(QPixmap());
}
