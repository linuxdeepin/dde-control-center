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
#include <cstdlib>

#include <com_deepin_daemon_display.h>

#include "controlcenterunittest.h"

using DBusDisplay = com::deepin::daemon::Display;

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

/**
 * @brief ControlCenterUnitTest::displayMode_check  检查显示模式
 */
void ControlCenterUnitTest::displayMode_check()
{
    QDBusInterface displayInter("com.deepin.daemon.Display", "/com/deepin/daemon/Display", "com.deepin.daemon.Display", QDBusConnection::sessionBus());
    srand((unsigned)time(NULL));
    int displayMode = rand() % 3;
    QString primaryScreen = displayInter.property("Primary").toString();
    auto ret = displayInter.call("SwitchMode", displayMode, primaryScreen);
    this->displayMode = displayInter.property("DisplayMode").toInt();
    switch (displayMode) {
    case 0:
        qDebug() << "Customised, please check.";
        break;
    case 1:
        qDebug() << "CopyMode, please check.";
        break;
    case 2:
        qDebug() << "ExpandMode, please check.";
        break;
    default:
        qDebug() << "Error!";
        break;
    }
    QCOMPARE(this->displayMode, displayMode);
}

void ControlCenterUnitTest::checkWindowCompositingEnable()
{
    QDBusInterface wmInter("com.deepin.wm",
                                 "/com/deepin/wm",
                                "org.freedesktop.DBus.Properties",
                                QDBusConnection::sessionBus());

    QDBusInterface wmPropertyInter("com.deepin.wm",
                                 "/com/deepin/wm",
                                "com.deepin.wm",
                                QDBusConnection::sessionBus());

    bool  compositingEnable = wmPropertyInter.property("compositingEnabled").toBool();
    bool  lastCompositingEnable = compositingEnable;

    QDBusMessage reply = wmInter.call("Set","com.deepin.wm","compositingEnabled", QVariant::fromValue(QDBusVariant(!compositingEnable)));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "reply.type() = " << reply.type();
    }

    QThread::sleep(4);
    compositingEnable = wmPropertyInter.property("compositingEnabled").toBool();          
    QCOMPARE(compositingEnable, !lastCompositingEnable);

    lastCompositingEnable = compositingEnable;
    reply = wmInter.call("Set","com.deepin.wm","compositingEnabled", QVariant::fromValue(QDBusVariant(!compositingEnable)));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << "reply.type() = " << reply.type();
    }
    QThread::sleep(4);

    compositingEnable = wmPropertyInter.property("compositingEnabled").toBool();
    QCOMPARE(compositingEnable, !lastCompositingEnable);
}

/**
 * @brief ControlCenterUnitTest::testProcessNumber
 * 启动10次，检测控制中心能否保持单例+控制中心能否正常退出
 */
void ControlCenterUnitTest::testProcessNumber()
{
    QProcess process;
    for (int i = 0; i < 10; ++i) {
        process.start("dde-control-center", QStringList() << "-s");
        process.waitForFinished(2000);
    }

    process.start("pidof", QStringList() << "dde-control-center");
    process.waitForFinished(2000);
    const QStringList& pids = QString(process.readAll()).trimmed().split(" ", QString::SkipEmptyParts);
    QCOMPARE(pids.size(), 1);

    process.start("dbus-send", QStringList() << "--session" << "--type=method_call" << "--print-reply"
                  << "--dest=com.deepin.dde.ControlCenter" << "/com/deepin/dde/ControlCenter"
                  << "com.deepin.dde.ControlCenter.exitProc");
    process.waitForFinished(2000);

    QCOMPARE(pids.size(), 0);
}

QTEST_APPLESS_MAIN(ControlCenterUnitTest)

#include "controlcenterunittest.moc"
