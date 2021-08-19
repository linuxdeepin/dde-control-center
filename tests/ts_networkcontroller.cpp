#include "networkdetails.h"
#include "wireddevice.h"
#include "wirelessdevice.h"
#include "networkcontroller.h"
#include "networkdevicebase.h"

#include <gtest/gtest.h>

using namespace dde::network;

class Tst_NetworkController : public testing::Test
{
public:
    void SetUp() override
    {
        m_controller = NetworkController::instance();
    }

    void TearDown() override
    {
        m_controller = nullptr;
    }

public:
    NetworkController *m_controller;
};

TEST_F(Tst_NetworkController, dslcontroll_test)
{
    NetworkController *controller = m_controller;

    auto onDeviceChange = [ = ](QList<NetworkDeviceBase *> changeDevices) {
        Q_UNUSED(changeDevices);

        QList<NetworkDeviceBase *> devices = controller->devices();
        for (NetworkDeviceBase *device : devices) {
            qInfo() << device->deviceName() << "," << device->realHwAdr();
            QObject::connect(device, &NetworkDeviceBase::deviceStatusChanged, [ = ](const DeviceStatus & deviceStatus) {
                int status = (int)deviceStatus;
                qInfo() << status;
            });
            QObject::connect(device, &NetworkDeviceBase::enableChanged, [ = ](const bool enabled) {
                if (enabled)
                    qInfo() << "device is enabled";
                else
                    qInfo() << "device is disabled";
            });
            if (device->deviceType() == DeviceType::Wired) {
                WiredDevice *wiredDevice = static_cast<WiredDevice *>(device);
                QList<WiredConnection *> connectionItems = wiredDevice->items();
                for (WiredConnection *conn : connectionItems) {
                    Connection *connection = conn->connection();
                    qInfo() << connection->path() << "," << connection->id() << "," << connection->ssid()
                               << connection->uuid();
                }
            } else if (device->deviceType() == DeviceType::Wireless) {
                WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
                QObject::connect(wirelessDevice, &WirelessDevice::networkAdded, [ = ] (QList<AccessPoints *> accessPoints) {
                    qInfo() << "new networkAdded:";
                    for (AccessPoints *ap : accessPoints)
                        qInfo() << "accessPoints added:" << ap->ssid();

                    qInfo() << "all network:";
                    QList<AccessPoints *> aps = wirelessDevice->accessPointItems();
                    for (AccessPoints *ap : aps)
                        qInfo() << ap->path() << "," << ap->ssid();

                    static bool isFirst = true;
                    if (isFirst && aps.size() > 0) {
                        AccessPoints *ap = aps[0];
                        wirelessDevice->connectNetwork(ap);
                        qInfo() << ap->path() << "," << ap->ssid();
                        isFirst = false;
                    }

                });

                QList<WirelessConnection *> connectionItems = wirelessDevice->items();
                for (WirelessConnection *conn : connectionItems) {
                    Connection *connection = conn->connection();
                    qInfo() << connection->path() << "," << connection->id()
                               << "," << connection->ssid() << "," << connection->uuid();
                }
            }
        }

        //Connectivity conectivity = m_controller->connectivity();
        QList<NetworkDetails *> details = controller->networkDetails();
        for (int i = 0; i < details.size(); i++) {
            NetworkDetails *detail = details[i];
            qInfo() << detail->name();
            QList<QPair<QString, QString>> items = detail->items();
            for (QPair<QString, QString> item : items)
                qInfo() << item.first << ":" << item.second;
        }
    };

    QObject::connect(m_controller, &NetworkController::deviceAdded, onDeviceChange);
    QObject::connect(m_controller, &NetworkController::deviceRemoved, onDeviceChange);
}
