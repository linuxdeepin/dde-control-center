// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/basiclistview.h"

using namespace dcc::widgets;

class Tst_BasicListView : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new BasicListView;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    BasicListView *obj = nullptr;
};

TEST_F(Tst_BasicListView, coverage)
{
    QStandardItemModel model;
    obj->setModel(&model);
    obj->onContentHeightChanged();
    obj->setAutoFitHeight(false);
    obj->sizeHint();
    obj->leaveEvent(nullptr);
}
