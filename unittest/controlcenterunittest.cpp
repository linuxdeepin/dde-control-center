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

#include "controlcenterunittest.h"

#include <com_deepin_daemon_display.h>

#include <QDBusInterface>
#include <QDBusMetaType>
#include <QDBusMessage>
#include <QDBusArgument>
#include <cstdlib>

using DBusDisplay = com::deepin::daemon::Display;

/*!
 * \brief SetPrimary 设置主屏
 * \param strDisplay
 */
TEST_F(ControlCenterUnitTest, SetPrimary)
{
    QString strDisplay = "HDMI-0";
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

//    EXPECT_EQ(strPrimary, strDisplay);
}

/*!
 * \brief testModules 测试控制中心每个模块的跳转是否正常
 */
TEST_F(ControlCenterUnitTest, testModules)
{
    QProcess *controlcenterProc = new QProcess();
    controlcenterProc->start("dde-control-center --show");
    connect(controlcenterProc, &QProcess::started, this, [=] {
        QDBusInterface inter("com.deepin.dde.ControlCenter",
                             "/com/deepin/dde/ControlCenter",
                             "com.deepin.dde.ControlCenter",
                             QDBusConnection::sessionBus());

        ASSERT_TRUE(inter.isValid());

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
        FAIL() << "control center error occurred" << error;
    });
    controlcenterProc->waitForFinished();
}

/*!
 * \brief inputDevieNum 测试显示的输出设备数量
 * \param num 预计显示的输出设备数
 * 未接入耳机时默认显示１个输出设备，测试通过
 * 当接入多个耳机时，会导致测试用例失败
 */
TEST_F(ControlCenterUnitTest, inputDevieNum)
{
    // 手动输入的参数
    int num = 1;

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
            if (portAvai == 2 || portAvai == 0 ) { // 0 Unknown 1 Not available 2 Available
                actualNum++;
            }
        }
    }

    EXPECT_EQ(actualNum, num);
}

/*!
 * \brief testBluetoothIsVisible 测试蓝牙是否正常显示
 */
TEST_F(ControlCenterUnitTest, testBluetoothIsVisible)
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

    EXPECT_EQ(reply, bluetoothState);
}

/*!
 * \brief displayMode_check 显示模式检查
 */
TEST_F(ControlCenterUnitTest, displayMode_check)
{
    QDBusInterface displayInter("com.deepin.daemon.Display", "/com/deepin/daemon/Display", "com.deepin.daemon.Display", QDBusConnection::sessionBus());
    srand(static_cast<unsigned>(time(nullptr)));
    int displayMode = rand() % 3;
    QString primaryScreen = displayInter.property("Primary").toString();
    auto ret = displayInter.call("SwitchMode", displayMode, primaryScreen);
    int displayModeReturn = displayInter.property("DisplayMode").toInt();
    switch (displayMode) {
    case 0:
        qDebug() <<  "Customised, please check.";
        break;
    case 1:
        qDebug() <<  "CopyMode, please check.";
        break;
    case 2:
        qDebug() <<  "ExpandMode, please check.";
        break;
    default:
        qDebug() <<  "Error!";
        break;
    }

    EXPECT_EQ(displayMode, displayModeReturn);
}

/*!
 * \brief checkWindowCompositingEnable 开启和关闭窗口特效检测
 */
TEST_F(ControlCenterUnitTest, checkWindowCompositingEnable)
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
        qDebug() <<  "reply.type() = " << reply.type();
    }

    QThread::sleep(4);
    compositingEnable = wmPropertyInter.property("compositingEnabled").toBool();
    EXPECT_EQ(compositingEnable, !lastCompositingEnable);

    lastCompositingEnable = compositingEnable;
    reply = wmInter.call("Set","com.deepin.wm","compositingEnabled", QVariant::fromValue(QDBusVariant(!compositingEnable)));
    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() <<  "reply.type() = " << reply.type();
    }
    QThread::sleep(4);

    compositingEnable = wmPropertyInter.property("compositingEnabled").toBool();
    EXPECT_EQ(compositingEnable, !lastCompositingEnable);
}

/*!
 * \brief testProcessNumber 启动10次，检测控制中心能否保持单例+控制中心能否正常退出
 */
TEST_F(ControlCenterUnitTest, testProcessNumber)
{
    QProcess process;
    for (int i = 0; i < 10; ++i) {
        process.start("dde-control-center", QStringList() << "-s");
        process.waitForFinished(2000);
    }

    process.start("pidof", QStringList() << "dde-control-center");
    process.waitForFinished(2000);
    const QStringList& pids = QString(process.readAll()).trimmed().split(" ", QString::SkipEmptyParts);
//    EXPECT_EQ(pids.size(), 1);

    process.start("dbus-send", QStringList() << "--session" << "--type=method_call" << "--print-reply"
                  << "--dest=com.deepin.dde.ControlCenter" << "/com/deepin/dde/ControlCenter"
                  << "com.deepin.dde.ControlCenter.exitProc");
    process.waitForFinished(2000);
    const QStringList& pidsKilled = QString(process.readAll()).trimmed().split(" ", QString::SkipEmptyParts);

    EXPECT_EQ(pidsKilled.size(), 0);
}

#include "controlcenterunittest.moc"
