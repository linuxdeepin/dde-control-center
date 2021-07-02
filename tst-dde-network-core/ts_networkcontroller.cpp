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
            qWarning() << device->deviceName() << "," << device->realHwAdr();
            QObject::connect(device, &NetworkDeviceBase::deviceStatusChanged, [ = ](const DeviceStatus & deviceStatus) {
                int status = (int)deviceStatus;
                qWarning() << status;
            });
            QObject::connect(device, &NetworkDeviceBase::enableChanged, [ = ](const bool enabled) {
                if (enabled)
                    qWarning() << "device is enabled";
                else
                    qWarning() << "device is disabled";
            });
            if (device->deviceType() == DeviceType::Wired) {
                WiredDevice *wiredDevice = static_cast<WiredDevice *>(device);
                QList<WiredConnection *> connectionItems = wiredDevice->items();
                for (WiredConnection *conn : connectionItems) {
                    Connection *connection = conn->connection();
                    qWarning() << connection->path() << "," << connection->id() << "," << connection->ssid()
                               << connection->uuid();
                }
            } else if (device->deviceType() == DeviceType::Wireless) {
                WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
                QList<WirelessConnection *> connectionItems = wirelessDevice->items();
                for (WirelessConnection *conn : connectionItems) {
                    Connection *connection = conn->connection();
                    qWarning() << connection->path() << "," << connection->id()
                               << "," << connection->ssid() << "," << connection->uuid();
                }
                QList<AccessPoints *> accessPoints = wirelessDevice->accessPointItems();
                for (AccessPoints *ap : accessPoints)
                    qWarning() << ap->path() << "," << ap->ssid();

                if (accessPoints.size() > 0) {
                    AccessPoints *ap = accessPoints[0];
                    wirelessDevice->connectNetwork(ap);
                    qWarning() << ap->path() << "," << ap->ssid();
                }
            }
        }

        //Connectivity conectivity = m_controller->connectivity();
        QList<NetworkDetails *> details = controller->networkDetails();
        for (int i = 0; i < details.size(); i++) {
            NetworkDetails *detail = details[i];
            qWarning() << detail->name();
            QList<QPair<QString, QString>> items = detail->items();
            for (QPair<QString, QString> item : items)
                qWarning() << item.first << ":" << item.second;
        }
    };

    QObject::connect(m_controller, &NetworkController::deviceAdded, onDeviceChange);
    QObject::connect(m_controller, &NetworkController::deviceRemoved, onDeviceChange);
}
