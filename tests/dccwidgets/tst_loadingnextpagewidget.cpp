// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/loadingnextpagewidget.h"

using namespace dcc::widgets;

class Tst_LoadingNextPageWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new LoadingNextPageWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    LoadingNextPageWidget *obj = nullptr;
};

TEST_F(Tst_LoadingNextPageWidget, coverage)
{
    obj->setLoading(1);
}
