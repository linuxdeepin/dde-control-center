#include <gtest/gtest.h>

#include "notification_dbus.h"
#include "src/plugin-notification/operation/notificationworker.h"

using namespace DCC_NAMESPACE;

class Tst_NotificationWorker : public testing::Test
{
public:
    void SetUp() override { }

    void TearDown() override { }

public:
};

TEST_F(Tst_NotificationWorker, init)
{
    NotificationModel model;
    NotificationWorker worker(&model);
}

TEST_F(Tst_NotificationWorker, FullTest)
{
    NotificationModel model;
    NotificationWorker worker(&model);
    worker.active(true);

    NotificationDBusProxy *dbus = worker.getDbusObject();
    dbus->SystemInfoChanged(0, QDBusVariant(true));
    dbus->SystemInfoChanged(1, QDBusVariant(true));
    dbus->SystemInfoChanged(2, QDBusVariant(true));
    dbus->SystemInfoChanged(3, QDBusVariant("07:00"));
    dbus->SystemInfoChanged(3, QDBusVariant("07:00"));
    dbus->SystemInfoChanged(4, QDBusVariant("18:00"));
    dbus->SystemInfoChanged(4, QDBusVariant("18:00"));
    dbus->SystemInfoChanged(5, QDBusVariant(true));

    dbus->AppInfoChanged("code", 0, QDBusVariant(true));
    dbus->AppInfoChanged("code", 1, QDBusVariant(true));
    dbus->AppInfoChanged("code", 2, QDBusVariant(true));
    dbus->AppInfoChanged("code", 3, QDBusVariant(true));
    dbus->AppInfoChanged("code", 4, QDBusVariant(true));
    dbus->AppInfoChanged("code", 5, QDBusVariant(true));
    dbus->AppInfoChanged("code", 6, QDBusVariant(true));

    dbus->AppRemovedSignal("code");

    worker.setAppSetting("code", 2, false);
    worker.setSystemSetting(0, false);
    worker.deactive();
}
