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

void ControlCenterUnitTest::testModules()
{
    QProcess *controlcenterProc = new QProcess();
    controlcenterProc->start("dde-control-center --show");
    connect(controlcenterProc, &QProcess::started, this, [=] {
        QDBusInterface inter("com.deepin.dde.ControlCenter",
                             "/com/deepin/dde/ControlCenter",
                             "com.deepin.dde.ControlCenter",
                             QDBusConnection::sessionBus());
        QVERIFY(inter.isValid());

        QStringList moduleList;
        moduleList << "accounts" << "bluetooth" << "commoninfo" << "datetime" << "defapp" << "display"
                   << "keyboard" << "mouse" << "network" << "notification" << "personalization" << "power"
                   << "sound" << "cloudsync" << "systeminfo" << "update";

        foreach(QString module, moduleList) {
            inter.call("ShowModule", module);
            QThread::sleep(1);
        }
        inter.call("exitProc");
    });
    connect(controlcenterProc, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error) {
        qDebug() << "control center error occurred: " << error;
        QFAIL("control center error occurred");
    });
    controlcenterProc->waitForFinished();
}

/**
 * @brief ControlCenterUnitTest::inputDevieNum 判断显示的输出设备个数
 * @param num 预计显示的输出设备数
 */
void ControlCenterUnitTest::inputDevieNum(int num)
{
    int actualNum = 0;
    QDBusInterface inter("com.deepin.daemon.Audio", "/com/deepin/daemon/Audio", "com.deepin.daemon.Audio", QDBusConnection::sessionBus(), this);
    QString info = inter.property("CardsWithoutUnavailable").toString();

    QJsonDocument doc = QJsonDocument::fromJson(info.toUtf8());
    QJsonArray jCards = doc.array();
    for (QJsonValue cV : jCards) {
        QJsonObject jCard = cV.toObject();
        QJsonArray jPorts = jCard["Ports"].toArray();

        for (QJsonValue pV : jPorts) {
            QJsonObject jPort = pV.toObject();
            const int portAvai = jPort["Available"].toInt();
            if (jPort["Direction"].toInt() != 1) //1输出, 2输入
                continue;
            if (portAvai == 2 || portAvai == 0 ) { // 0 Unknow 1 Not available 2 Available
                actualNum++;
            }
        }
    }

    QCOMPARE(actualNum, num);
}

void ControlCenterUnitTest::testBluetoothIsVisible()
{
    QDBusInterface inter("com.deepin.dde.ControlCenter",
                         "/com/deepin/dde/ControlCenter",
                         "com.deepin.dde.ControlCenter",
                         QDBusConnection::sessionBus());
    QDBusReply<bool> reply = inter.call("isModuleAvailable","bluetooth");

    QDBusInterface bluetoothInter("com.deepin.daemon.Bluetooth",
                         "/com/deepin/daemon/Bluetooth",
                         "com.deepin.daemon.Bluetooth",
                         QDBusConnection::sessionBus());
    auto bluetoothState = bluetoothInter.property("State").toInt();

    QCOMPARE(reply,bluetoothState);
}

QTEST_APPLESS_MAIN(ControlCenterUnitTest)

#include "controlcenterunittest.moc"
