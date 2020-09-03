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

class ControlCenterUnitTest : public QObject
{
    Q_OBJECT

public:
    ControlCenterUnitTest();
    ~ControlCenterUnitTest();

private slots:
    void SetPrimary(QString strDisplay = "HDMI-0");         //设置主屏
    // 测试控制中心每个模块的跳转是否正常
    void testModules();
};

#endif // CONTROLCENTER_UNIT_TEST_H
