// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/multiselectlistview.h"

using namespace dcc::widgets;

class Tst_MultiSelectListView : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new MultiSelectListView;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    MultiSelectListView *obj = nullptr;
};

TEST_F(Tst_MultiSelectListView, coverage)
{
    obj->resetStatus(QModelIndex());
}
