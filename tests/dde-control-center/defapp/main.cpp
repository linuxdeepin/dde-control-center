#include "defapp_dbus.h"

#include <QApplication>
#include <QDebug>
#include <QProcess>

#include <gtest/gtest.h>

#ifdef QT_DEBUG
#include <sanitizer/asan_interface.h>
#endif

int main(int argc, char **argv)
{
    QProcess process;
    QString cmd = "dbus-daemon --session --print-address";
    process.start(cmd);
    process.waitForReadyRead();

    QString path = process.readAllStandardOutput().simplified();

    setenv("DBUS_SESSION_BUS_ADDRESS", path.toStdString().data(), 1);
    setenv("QT_QPA_PLATFORM", "offscreen", 1);

    QApplication app(argc, argv);

    QDBusConnection conn = QDBusConnection::sessionBus();
    bool bOk = conn.registerService(DEFAPP_SERVICE_NAME);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }

    Defapp_DBUS service;
    bOk = conn.registerObject(DEFAPP_SERVICE_PATH, &service, QDBusConnection::ExportAllContents);
    if (!bOk) {
        QDBusError err = conn.lastError();
        qWarning() << err.name() << ", " << err.message();
        process.close();
        return -1;
    }

    ::testing::InitGoogleTest(&argc, argv);

    int result = RUN_ALL_TESTS();

#ifdef QT_DEBUG
    __sanitizer_set_report_path("asan_defapp.log");
#endif

    process.close();
    return result;
}
