// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/contentwidget.h"

using namespace dcc;

class Tst_ContentWidget : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new ContentWidget;
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    ContentWidget *obj = nullptr;
};

TEST_F(Tst_ContentWidget, coverage)
{
    obj->setTitle("aaa");
    obj->sendBackSignal();
    obj->content();
    obj->setContent(new QWidget());
    obj->scrollTo(5);
}
