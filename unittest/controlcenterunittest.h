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
    enum DisplayMode {
        Customised = 0,
        CopyMode = 1,
        ExpandMode = 2
    };

    ControlCenterUnitTest();
    ~ControlCenterUnitTest();

private slots:

    /*!
     * \brief SetPrimary 设置主屏
     * \param strDisplay
     */
    void SetPrimary(QString strDisplay = "HDMI-0");

    /*!
     * \brief testModules 测试控制中心每个模块的跳转是否正常
     */
    void testModules();

    /*!
     * \brief inputDevieNum 测试显示的输出设备数量
     * \param num 预计显示的输出设备数
     * 未接入耳机时默认显示１个输出设备，测试通过
     * 当接入多个耳机时，会导致测试用例失败
     */
    void inputDevieNum(int num = 1);

    /*!
     * \brief testBluetoothIsVisible 测试蓝牙是否正常显示
     */
    void testBluetoothIsVisible();

    /*!
     * \brief displayMode_check 显示模式检查
     */
    void displayMode_check();

    /*!
     * \brief checkWindowCompositingEnable 开启和关闭窗口特效检测
     */
    void checkWindowCompositingEnable();

    /*!
     * \brief testProcessNumber 启动10次，检测控制中心能否保持单例+控制中心能否正常退出
     */
    void testProcessNumber();

private:
    int displayMode;
};

#endif // CONTROLCENTER_UNIT_TEST_H
