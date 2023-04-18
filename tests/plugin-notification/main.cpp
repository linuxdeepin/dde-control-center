//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "notification_dbus.h"

#include <QApplication>
#include <QDebug>
#include <QProcess>

#include <gtest/gtest.h>

#ifdef QT_DEBUG
#  ifdef USE_ASAN
#    include <sanitizer/asan_interface.h>
#  endif
#endif
int main(int argc, char **argv)
{
    QProcess process;
    QString cmd = "dbus-daemon";
    process.start(cmd, {"--session", "--print-address"});
    process.waitForReadyRead();

    QString path = process.readAllStandardOutput().simplified();

    setenv("DBUS_SESSION_BUS_ADDRESS", path.toStdString().data(), 1);
    setenv("QT_QPA_PLATFORM", "offscreen", 1);
    qDebug() << getenv("DBUS_SESSION_BUS_ADDRESS");

    QApplication app(argc, argv);

    QDBusConnection conn = QDBusConnection::sessionBus();
    bool bOk = conn.registerService(NOTIFICATION_SERVICE_NAME);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }
    Notification_DBUS service;
    bOk = conn.registerObject(NOTIFICATION_SERVICE_PATH, &service, QDBusConnection::ExportAllContents);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

#ifdef QT_DEBUG
#  ifdef USE_ASAN
    __sanitizer_set_report_path("dccwidgets_asan.log");
#  endif
#endif

    process.close();

    return result;
}
