// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "datetime_dbus.h"

#include <QApplication>
#include <QDebug>
#include <QProcess>

#include <gtest/gtest.h>

#ifdef QT_DEBUG
#include <sanitizer/asan_interface.h>
#endif

// To Finished
int main(int argc, char **argv)
{
    QProcess process;
    QString cmd = "dbus-daemon --session --print-address";
    process.start(cmd);
    process.waitForReadyRead();

    QString path = process.readAllStandardOutput().simplified();

    setenv("DBUS_SESSION_BUS_ADDRESS", path.toStdString().data(), 1);
    setenv("QT_QPA_PLATFORM", "offscreen", 1);
    qDebug() << getenv("DBUS_SESSION_BUS_ADDRESS");

    QApplication app(argc, argv);

    QDBusConnection conn = QDBusConnection::sessionBus();
    bool bOk = conn.registerService(DATETIME_SERVICE_NAME);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }
    TimeDate service;
    bOk = conn.registerObject(DATETIME_SERVICE_PATH, &service, QDBusConnection::ExportAllContents);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

#ifdef QT_DEBUG
    __sanitizer_set_report_path("asan_datetime.log");
#endif

    process.close();
    return result;
}
