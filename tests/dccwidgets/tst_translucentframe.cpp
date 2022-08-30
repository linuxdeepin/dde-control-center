// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <gtest/gtest.h>

#include <QStandardItemModel>

#include "../../include/widgets/translucentframe.h"

using namespace dcc::widgets;

class Tst_TranslucentFrame : public testing::Test
{
public:
    void SetUp() override
    {
        obj = new TranslucentFrame();
    }

    void TearDown() override
    {
        delete obj;
        obj = nullptr;
    }

public:
    TranslucentFrame *obj = nullptr;
};

TEST_F(Tst_TranslucentFrame, coverage)
{

}
