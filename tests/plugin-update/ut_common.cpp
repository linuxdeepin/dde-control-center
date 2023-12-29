// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <gtest/gtest.h>

#include "common.h"

#include <QString>

#include <vector>

class Tst_UpdateCommon : public testing::Test
{
};

TEST_F(Tst_UpdateCommon, regex)
{
    QString toRegex = "abc123eft-123c-1.23";

    std::vector<double> result = { 123, -123, -1.23 };

    std::vector<double> getData = getNumListFromStr(toRegex);
    ASSERT_EQ(getData.size(), 3);
    ASSERT_EQ(getData, result);
}
