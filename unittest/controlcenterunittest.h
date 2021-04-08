/*
 * Copyright (C) 2018 ~ 2028 Uniontech Technology Co., Ltd.
 *
 * Author:     huangweihua <huangweihua@uniontech.com>
 *
 * Maintainer: huangweihua <huangweihua@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef CONTROLCENTER_UNIT_TEST_H
#define CONTROLCENTER_UNIT_TEST_H
#include <QObject>
#include <gtest/gtest.h>

class ControlCenterUnitTest : public QObject, public ::testing::Test
{
    Q_OBJECT

public:
    enum DisplayMode {
        Customised = 0,
        CopyMode = 1,
        ExpandMode = 2
    };

    ControlCenterUnitTest() {}
    virtual ~ControlCenterUnitTest() {}

    virtual void SetUp() {}

    virtual void TearDown() {}

private:
    int displayMode;
};

#endif // CONTROLCENTER_UNIT_TEST_H
