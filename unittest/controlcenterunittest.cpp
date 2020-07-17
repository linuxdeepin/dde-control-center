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

#include <QtTest>
#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusMessage>
#include <QDBusArgument>

#include <com_deepin_daemon_display.h>

#include "controlcenterunittest.h"

ControlCenterUnitTest::ControlCenterUnitTest()
{

}

ControlCenterUnitTest::~ControlCenterUnitTest()
{

}
/**
 * @brief ControlCenterUnitTest::SetPrimary   设置主屏
 */
void ControlCenterUnitTest::SetPrimary(QString strDisplay)
{
    QString strPrimary;

    {
        QDBusInterface inter("com.deepin.daemon.Display",
                             "/com/deepin/daemon/Display",
                             "com.deepin.daemon.Display",
                             QDBusConnection::sessionBus());
        auto reply = inter.call("SetPrimary", strDisplay);
    }

    {
        QDBusInterface inter("com.deepin.daemon.Display",
                             "/com/deepin/daemon/Display",
                             "com.deepin.daemon.Display",
                             QDBusConnection::sessionBus());
        auto reply = inter.property("Primary");
        strPrimary = reply.toString();
        qDebug() << reply.toString();
    }

    QCOMPARE(strPrimary, strDisplay);
}

QTEST_APPLESS_MAIN(ControlCenterUnitTest)

#include "controlcenterunittest.moc"
